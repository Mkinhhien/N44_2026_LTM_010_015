# Đánh giá hiệu năng TCP NewReno và TCP Cubic trên mạng nút thắt cổ chai (ns-3)

Dự án mô phỏng so sánh khả năng kiểm soát tắc nghẽn và thông lượng giữa **TCP NewReno** và **TCP Cubic** trên mô hình mạng Dumbbell (nút thắt cổ chai 10 Mbps, độ trễ 50 ms) bằng **ns-3.43**.

## Cấu trúc thư mục
- `tcp-comparison.cc`: Kịch bản mô phỏng C++ viết bằng API của ns-3.
- `plot.py`: Script Python (`pandas`, `matplotlib`) xử lý dữ liệu trace và xuất biểu đồ.
- `TcpNewReno-cwnd.data` & `TcpCubic-cwnd.data`: Dữ liệu biến thiên Congestion Window (`cWnd`) theo thời gian.
- `tcp_comparison_result.png`: Biểu đồ so sánh kết quả mô phỏng.

## Hướng dẫn chạy
1. Copy `tcp-comparison.cc` vào thư mục `scratch/` của bộ cài `ns-3`.
2. Chạy mô phỏng cho hai biến thể TCP:
   ```bash
   ./ns3 run "tcp-comparison --tcpVariant=TcpNewReno"
   ./ns3 run "tcp-comparison --tcpVariant=TcpCubic"
python3 plot.py
# N44_2026_LTM_010_015
# N44_2026_LTM_010_015
