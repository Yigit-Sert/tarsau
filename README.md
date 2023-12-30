# Tarsau Archive Utility

## Overview

"Tarsau" is a C-based archive utility designed for the Linux (or Unix) operating system. It provides a mechanism to combine multiple text files into a single archive file without compression. The utility aims to offer simplicity and reliability for archiving purposes.

## Installation

1. **Clone the repository:**
git clone https://github.com/Yigit-Sert/tarsau.git

markdown
Copy code

2. **Navigate to the project directory:**
cd tarsau

markdown
Copy code

3. **Compile the program:**
gcc -o tarsau tarsau.c

perl
Copy code

## Usage

### Creating an Archive

To create an archive, use the `-b` option followed by input file names and specify the output archive file name with `-o`:

tarsau –b t1 t2 t3 t4.txt t5.dat –o s1.sau

vbnet
Copy code

### Extracting from an Archive

To extract files from an existing archive, use the `-a` option followed by the archive file name and the directory name:

tarsau –a s1.sau d1

sql
Copy code

## Archive File Format

The ".sau" archive file format consists of two main sections:

1. **Organization (Contents) Section**: Contains meta-information about the archived files.
   - The first 10 bytes indicate the size of this section.
   - Records are separated by '|', and fields within a record are separated by commas: `|File name, permissions, size|`

2. **Archived Files**: Follows the organization section and contains the actual file data in ASCII format without delimiters. The last character signifies the end of the file.

## Limitations

- Supports only text files (ASCII, 1 byte per character).
- Maximum of 32 input files.
- Total input file size limited to 200 MBytes.
- Inappropriate file formats will result in clean error messages.

## Contributing

Contributions are welcome! Please submit pull requests or open issues for any improvements or fixes.
