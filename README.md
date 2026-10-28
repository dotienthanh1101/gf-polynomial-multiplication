# Nhân đa thức trong GF(2^4) và GF(2^8)

Bài tập cài đặt phép nhân hai phần tử trong trường hữu hạn bằng C++17.
Mỗi phần tử được biểu diễn bằng một đa thức hệ số nhị phân có bậc nhỏ hơn n.
Chương trình nhân bằng dịch bit, XOR và rút gọn theo đa thức bất khả quy m(x).

## 1. Đa thức được chọn

| Trường | Đa thức bất khả quy m(x) | Nhị phân | Hex |
| --- | --- | --- | --- |
| GF(2^4) | x^4 + x + 1 | 10011 | 0x13 |
| GF(2^8) | x^8 + x^4 + x^3 + x + 1 | 100011011 | 0x11B |

Đề bài chưa ấn định đa thức cụ thể nên chương trình chọn hai đa thức trên làm mặc định.
Nếu giảng viên chỉ định đa thức khác, truyền đầy đủ đa thức đó qua đối số `modulus`.
Ví dụ, chương trình chấp nhận `0x11D = x^8 + x^4 + x^3 + x^2 + 1` cho GF(2^8).

**Ví dụ đa thức không hợp lệ:**
`x^8 + x^4 + x^3 + x^2 + x + 1 = 0x11F` là đa thức khả quy vì m(1) = 0.
Đa thức này không xác định một trường GF(2^8); chương trình sẽ từ chối.
Thuật ngữ cần dùng ở đây là đa thức bất khả quy bậc n trên GF(2).

## 2. Cơ sở toán học

Với a(x), b(x) có bậc nhỏ hơn n, kết quả cần tính là:

```text
c(x) = a(x) * b(x) mod m(x)
```

Bit thứ i của số nguyên là hệ số của x^i. Ví dụ:

```text
0xB = 1011 (nhị phân) = x^3 + x + 1
```

Trong GF(2), 1 + 1 = 0 nên cộng và trừ đa thức đều được thực hiện bằng XOR.
Nhân một đa thức với x tương đương dịch trái một bit. Nếu xuất hiện hạng x^n,
XOR với m(x) để khử hạng đó. Không dùng phép nhân số nguyên hoặc phép `%`
số nguyên để thay thế phép nhân và phép chia đa thức.

## 3. Giải thuật

```text
Input: a, b trong [0, 2^n - 1], m bất khả quy bậc n
result = 0
Lặp n lần:
    Nếu bit thấp nhất của b là 1: result = result XOR a
    carry = bit thứ n-1 của a
    a = a << 1
    Nếu carry bằng 1: a = a XOR m
    a = a AND (2^n - 1)
    b = b >> 1
Trả về result
```

Ở vòng lặp i, `a` biểu diễn `a_ban_dau(x) * x^i mod m(x)`, còn bit thấp nhất
của `b` là hệ số thứ i của đa thức b ban đầu. XOR các giá trị `a` tương ứng
với các hệ số bằng 1 cho kết quả tích modulo m(x).

Với n = 4 hoặc 8, các giá trị nằm trong một từ máy: phép nhân cần n vòng lặp,
tức O(n) thời gian và O(1) bộ nhớ phụ. Kiểm tra tính bất khả quy được thực hiện
khi khởi tạo trường, bằng cách thử các ước monic có bậc từ 1 đến n/2.
Phần kiểm tra này tách biệt với n vòng lặp của phép nhân.

## 4. Cấu trúc mã nguồn

```text
gf2n.hpp             Biểu diễn đa thức, kiểm tra bất khả quy, phép nhân
main.cpp             Nhận đầu vào và in kết quả, hỗ trợ --trace
tests/test_gf2n.cpp   Kiểm thử độc lập
Makefile             Biên dịch và chạy kiểm thử
README.md            Thuyết minh và hướng dẫn
```

Không cần cài thư viện ngoài.

## 5. Biên dịch và chạy

Yêu cầu trình biên dịch hỗ trợ C++17, ví dụ GCC/MinGW g++.

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic main.cpp -o gf_multiply
```

Trên Linux/macOS:

```bash
./gf_multiply
./gf_multiply 4 0xB 0x7 --trace
./gf_multiply 8 0x57 0x83 --trace
./gf_multiply 8 0x57 0x83 0x11D --trace
```

Trên Windows PowerShell, thêm đuôi `.exe` khi biên dịch và chạy:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic main.cpp -o gf_multiply.exe
.\gf_multiply.exe 4 0xB 0x7 --trace
.\gf_multiply.exe 8 0x57 0x83 --trace
```

Cú pháp chung:

```text
gf_multiply n a b [modulus] [--trace]
```

- `n`: 4 hoặc 8.
- `a`, `b`: số thập phân, số hex bắt đầu bằng `0x`, hoặc số nhị phân bắt đầu bằng `0b`.
- `modulus`: toàn bộ đa thức, bao gồm bit x^n; mặc định là `0x13` hoặc `0x11B`.
- `--trace`: in trạng thái trước mỗi lần dịch và kết quả sau bước XOR.
- Không có đối số: chạy hai ví dụ mẫu.
- Đầu vào không hợp lệ: báo lỗi và trả mã thoát 1.

Ví dụ nhập nhị phân:

```bash
./gf_multiply 4 0b1011 0b0111
```

## 6. Ví dụ tính toán

### GF(2^4)

```text
a(x) = x^3 + x + 1 = 0xB
b(x) = x^2 + x + 1 = 0x7
m(x) = x^4 + x + 1 = 0x13
```

Nhân đa thức chưa rút gọn:

```text
a(x)b(x) = x^5 + x^4 + 1
x^5 + x^4 + 1 = (x + 1)m(x) + x^2
```

Vậy `0xB * 0x7 = 0x4`, tương ứng c(x) = x^2.

### GF(2^8)

```text
a = 0x57, b = 0x83, m = 0x11B
c = 0xC1 = 193 = x^7 + x^6 + 1
```

Khi bật `--trace`:

| i | a | b | b & 1 | Kết quả sau XOR | Có hạng x^8 khi dịch a? |
| --- | --- | --- | --- | --- | --- |
| 0 | 0x57 | 0x83 | 1 | 0x57 | 0 |
| 1 | 0xAE | 0x41 | 1 | 0xF9 | 1 |
| 2 | 0x47 | 0x20 | 0 | 0xF9 | 0 |
| 3 | 0x8E | 0x10 | 0 | 0xF9 | 1 |
| 4 | 0x07 | 0x08 | 0 | 0xF9 | 0 |
| 5 | 0x0E | 0x04 | 0 | 0xF9 | 0 |
| 6 | 0x1C | 0x02 | 0 | 0xF9 | 0 |
| 7 | 0x38 | 0x01 | 1 | 0xC1 | 0 |

## 7. Kiểm thử

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic tests/test_gf2n.cpp -o test_gf2n
./test_gf2n
```

Trên Windows dùng `-o test_gf2n.exe` và chạy `.\test_gf2n.exe`.
Nếu có GNU Make, có thể chạy `make all test`.

Nội dung kiểm thử:

- Đối chiếu toàn bộ 256 cặp phần tử của GF(2^4), modulo 0x13.
- Đối chiếu toàn bộ 65.536 cặp phần tử của GF(2^8), modulo 0x11B.
- Đối chiếu thêm toàn bộ 65.536 cặp với modulo 0x11D.
- Hàm tham chiếu nhân từng hệ số để tạo tích đầy đủ, rồi chia đa thức lấy dư.
  Hàm này không gọi lại phép nhân hoặc hàm lấy dư của chương trình chính.
- Kiểm tra tính giao hoán, nhân với 0 và 1, miền kết quả và sự tồn tại của
  nghịch đảo cho từng phần tử khác 0.
- Kiểm tra tính phân phối và kết hợp: mọi bộ ba của GF(2^4), các bộ ba lấy mẫu
  xác định của GF(2^8).
- Từ chối đa thức khả quy, đa thức sai bậc, bậc trường không hỗ trợ và phần tử vượt miền.

Kết quả mong đợi:

```text
PASS GF(2^4) modulo 0x13: 256 cap nhan.
PASS GF(2^8) modulo 0x11B: 65536 cap nhan.
PASS GF(2^8) modulo 0x11D: 65536 cap nhan.
Tat ca kiem thu deu dat.
```

## 8. Thông tin repository

- Tài khoản GitHub: `dotienthanh1101`.
- Repository: https://github.com/dotienthanh1101/gf-polynomial-multiplication
- Ngôn ngữ: C++17.
- Bài nộp gồm mã nguồn, kiểm thử và thuyết minh thuật toán trong README này.
