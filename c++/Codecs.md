**ASCII:** unaccented English letters, from 32-127. Codes below 32 were called unprintable and were used for control characters. like 7 make computer beep.

**OEM character set:** In the ANSI standard, the code below 128 is same as before. For character above 128, it depends where you lived. The different systems are called **code pages**.

**DBCS**: double byte character set: Some letter stored in one byte and others took two.

**Unicode**: Every platonic letter in every alphabet is assigned a magic number by the Unicode consortium like U+0041 (A). This magic number is called **code point**. This number is hexadecimal.

**Encodings**

Say Hello represented by Unicode is U+0048 U+0065 U+006C U+006C U+006F. **Encoding decides how to store this code points in memory or represent it in an email message.**

In order to mark high endian or low endian mode, store FEFF at the beginning of every Unicode string. The FEFF was called **Unicode Byte Order mark**. 

**UTF-8**: UTF-8 was another system for storing you string of Unicode code points. In UTF-8, every code point from 0-127 is stored in a single byte. Only code points 128 and above are stored using 2, 3, up to 6 bytes.

**UTF-16 (UCS-2)**: Store in two byte, use Unicode Byte Order mark to mark little endian and high endian.

**UTF-32 (UCS-4)**: Stores each code point in 4 bytes. Waste of memory a lot.

**It does not make sense to have a string without knowing what encoding it use**.

For UCS-2 (two byte) Unicode, in the c++ code declare strings as wchar_t instead of char and use the wcs instead of str function. (wcslen->strlen). To create a literal UCS-2 string in c code put L in front of string like: **L"Hello"**.