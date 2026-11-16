#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TcpComparisonSimple");

static void
CwndTracer(Ptr<OutputStreamWrapper> stream, uint32_t oldVal, uint32_t newVal)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << " " << newVal << std::endl;
}

int main(int argc, char *argv[])
{
    std::string tcpVariant = "TcpCubic";
    CommandLine cmd(__FILE__);
    cmd.AddValue("tcpVariant", "Chon loai TCP: TcpNewReno hoac TcpCubic", tcpVariant);
    cmd.Parse(argc, argv);

    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::" + tcpVariant));

    NodeContainer nodes;
    nodes.Create(4);

    PointToPointHelper accessLink;
    accessLink.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    accessLink.SetChannelAttribute("Delay", StringValue("2ms"));

    PointToPointHelper bottleneckLink;
    bottleneckLink.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    bottleneckLink.SetChannelAttribute("Delay", StringValue("50ms"));
    bottleneckLink.SetQueue("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue(QueueSize("20p")));

    NetDeviceContainer dev01 = accessLink.Install(nodes.Get(0), nodes.Get(1));
    NetDeviceContainer dev12 = bottleneckLink.Install(nodes.Get(1), nodes.Get(2));
    NetDeviceContainer dev23 = accessLink.Install(nodes.Get(2), nodes.Get(3));

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    address.Assign(dev01);

    address.SetBase("10.1.2.0", "255.255.255.0");
    address.Assign(dev12);

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer if23 = address.Assign(dev23);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    uint16_t port = 9000;
    BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(if23.GetAddress(1), port));
    source.SetAttribute("MaxBytes", UintegerValue(0));
    ApplicationContainer sourceApps = source.Install(nodes.Get(0));
    sourceApps.Start(Seconds(1.0));
    sourceApps.Stop(Seconds(20.0));

    PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps = sink.Install(nodes.Get(3));
    sinkApps.Start(Seconds(0.0));
    sinkApps.Stop(Seconds(20.0));

    AsciiTraceHelper asciiTraceHelper;
    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream(tcpVariant + "-cwnd.data");
    Simulator::Schedule(Seconds(1.0001), [&]() {
        Config::ConnectWithoutContext(
            "/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",
            MakeBoundCallback(&CwndTracer, stream));
    });

    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    Simulator::Stop(Seconds(20.0));
    Simulator::Run();

    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    for (auto const &entry : stats)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(entry.first);
        if (t.sourceAddress == "10.1.1.1")
        {
            double duration = entry.second.timeLastRxPacket.GetSeconds() - entry.second.timeFirstTxPacket.GetSeconds();
            double throughputKbps = (entry.second.rxBytes * 8.0) / (duration * 1024);

            std::cout << "\n================ KET QUA: " << tcpVariant << " ================" << std::endl;
            std::cout << "Throughput: " << throughputKbps << " Kbps" << std::endl;
            std::cout << "Tong so goi gui: " << entry.second.txPackets << std::endl;
            std::cout << "Tong so goi nhan: " << entry.second.rxPackets << std::endl;
            std::cout << "So goi bi drop: " << entry.second.lostPackets << std::endl;
            std::cout << "Ti le mat goi: " << (double)entry.second.lostPackets / entry.second.txPackets * 100 << " %" << std::endl;
            std::cout << "========================================================\n" << std::endl;
        }
    }

    Simulator::Destroy();
    return 0;
}
