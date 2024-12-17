## Overview

This script is designed to automate the process of checking the correctness of a program on a set of input files.
It runs the program on each input file by piping the contents of the file to the program's stdin and then checks the
correctness of the output.

## Requirements:

- Python (version 3.x recommended)
- timeout utility available in the PATH (usually comes with UNIX-based OS, for Windows use WSL)

## How to Execute the exact script:

Execute the script using Python:

```bash
python3 checker.py exact <executable> [--time_limit <time_in_seconds>] [--max_time_limit_exceeded <max_limit>]
```

Where:

- `executable`: Path to the executable you want to run on the input files or a command to execute the program
- `time_limit`: (Optional) The time limit for each execution. Default is 60 seconds.
- `max_limit`: (Optional) The maximum number of times the time limit can be exceeded before the script stops.
  Default is 10.

## Examples:

A few examples for some common languages:

### Java

```bash
python3 checker.py exact "java -jar my_java.jar"
```

### C

```bash
python3 checker.py exact /home/algeng/uni/my_c_program
```

### Python

```bash
python3 checker.py exact "python3 program.py"
```

## How to Execute the kernel script:

```bash
python3 checker.py kernel <kernel_executable> "java -jar heu.jar" <lifting_executable> [--time_limit <time_in_seconds>] [--max_time_limit_exceeded <max_limit>]
```

## How to Execute the lower bound script:

```bash
python3 checker.py lb <lb_executable> [--time_limit <time_in_seconds>] [--max_time_limit_exceeded <max_limit>]
```

## How to Execute the upper bound script:

```bash
python3 checker.py ub <ub_executable> [--time_limit <time_in_seconds>] [--max_time_limit_exceeded <max_limit>]
```

## Output Format:

The script will print the results in the following CSV format:

```csv
file,status,(points),time,return,stderr
```

Where:

- `file`: The name of the input file
- `status`:  Status of execution (OK, Wrong, timelimit).
- `points`: (Optional) The number of points the program got for this input file. Not available in the exact script.
- `time`: Execution time in seconds.
- `return`: Return code of the program.
- `stderr`: The output of the program on stderr.