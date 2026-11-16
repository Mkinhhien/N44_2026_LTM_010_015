import matplotlib.pyplot as plt
import pandas as pd

reno = pd.read_csv("TcpNewReno-cwnd.data", sep=" ", names=["Time", "cWnd"])
cubic = pd.read_csv("TcpCubic-cwnd.data", sep=" ", names=["Time", "cWnd"])

# Kéo dài giá trị cWnd cuối cùng đến hết thời gian mô phỏng (20.0s) nếu cWnd đã bão hòa đứng yên
reno = pd.concat([reno, pd.DataFrame({"Time": [20.0], "cWnd": [reno["cWnd"].iloc[-1]]})], ignore_index=True)
cubic = pd.concat([cubic, pd.DataFrame({"Time": [20.0], "cWnd": [cubic["cWnd"].iloc[-1]]})], ignore_index=True)

plt.figure(figsize=(10, 5))
plt.plot(reno["Time"], reno["cWnd"] / 1024, label="TCP NewReno", linestyle="--", color="blue")
plt.plot(cubic["Time"], cubic["cWnd"] / 1024, label="TCP Cubic", color="red", linewidth=1.8)
plt.xlabel("Thời gian (giây)")
plt.ylabel("Cửa sổ tắc nghẽn cWnd (KB)")
plt.title("So sánh biến thiên Congestion Window: TCP NewReno vs TCP Cubic")
plt.xlim(1, 20)
plt.grid(True, linestyle=":", alpha=0.7)
plt.legend()
plt.tight_layout()
plt.savefig("tcp_comparison_result.png", dpi=300)
print("Xong! Da cap nhat anh tai: tcp_comparison_result.png")
