# Homework 01 - Longest Strictly Increasing Subsequence Length

### Problem Statement:

Write a complete C program that reads a sequence of integers from the user (input ends when -1 is entered) and computes the length of the longest **strictly increasing** subsequence.
In Persian: برنامه کاملی به زبان سی بنویسید که دنباله‌ای از اعداد صحیح را از کاربر بگیرد (ورودی با ورود عدد -1 خاتمه پیدا می‌کند) و طول بزرگ‌ترین زیرتوالی اکیداً صعودی را محاسبه کند

**Notes:**
- The input continues until -1 is entered (-1 is not part of the sequence and only signals termination).
- A strictly increasing subsequence means each element is **greater than** the previous one (e.g., 2 < 4 < 6 < 8).
- Only the **length** of the longest such subsequence is required (not the subsequence itself).

**Example 1:**

Input: 3 5 7 2 4 6 8 1 -1
Expected Output: 4
(Explanation: The longest strictly increasing subsequence is 2, 4, 6, 8 → length 4)

**Example 2:**

Input: -2 -3 -4 -5 1 2 3 4 -3 0 1 10 -1
Expected Output: 5
(Explanation: The longest strictly increasing subsequence is -3, -1, 0, 1, 10 → length 5)
