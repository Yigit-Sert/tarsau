Description
Tarsau Archive Utility

The "tarsau" program is a simple archive utility written in C for the Linux (or Unix) operating system. Unlike traditional archive utilities that compress files, "tarsau" focuses solely on combining multiple text files into a single text-based archive file, similar to how tar, rar, or zip works but without compression.

Features:
Combine multiple text files into a single archive.
Supports a maximum of 32 input files.
The total size of input files cannot exceed 200 MBytes.
Supports archiving directories to a specific location.
Maintains file permissions during the archiving process.
Clean error handling and messages for various scenarios.
Readme.md
Tarsau Archive Utility
Overview
"Tarsau" is a C-based archive utility designed for the Linux (or Unix) operating system. It provides a mechanism to combine multiple text files into a single archive file without compression. The utility aims to offer simplicity and reliability for archiving purposes.

Installation
Clone the repository:

bash
Copy code
git clone https://github.com/yourusername/tarsau.git
Navigate to the project directory:

bash
Copy code
cd tarsau
Compile the program:

Copy code
gcc -o tarsau tarsau.c
Usage
Creating an Archive
To create an archive, use the -b option followed by input file names and specify the output archive file name with -o:

css
Copy code
tarsau –b t1 t2 t3 t4.txt t5.dat –o s1.sau
Extracting from an Archive
To extract files from an existing archive, use the -a option followed by the archive file name and the directory name:

css
Copy code
tarsau –a s1.sau d1
Archive File Format
The ".sau" archive file format consists of two main sections:

Organization (Contents) Section: Contains meta-information about the archived files.

The first 10 bytes indicate the size of this section.
Records are separated by '|', and fields within a record are separated by commas: |File name, permissions, size|
Archived Files: Follows the organization section and contains the actual file data in ASCII format without delimiters. The last character signifies the end of the file.

Limitations
Supports only text files (ASCII, 1 byte per character).
Maximum of 32 input files.
Total input file size limited to 200 MBytes.
Inappropriate file formats will result in clean error messages.
Contributing
Contributions are welcome! Please submit pull requests or open issues for any improvements or fixes.

License
