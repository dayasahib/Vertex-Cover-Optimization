import argparse
import os
import re
import time

import subprocess


def run_command_with_limit(cmd, input_file, timeout):
    """
    Run a command with a time limit and redirecting stdin from a file.

    cmd: string, the command to run
    input_file: string, path to the file for stdin redirection
    timeout: int, time limit in seconds

    Returns: tuple containing return code, execution time, stdout, stderr,
             and a boolean indicating if the process was terminated due to a timeout
    """

    try:
        start_time = time.time()

        # If cmd is a string, split it into a list for subprocess.run
        if isinstance(cmd, str):
            cmd = cmd.split()

        with open(input_file, 'r') as f:
            completed_process = subprocess.run(cmd, stdin=f, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                               timeout=timeout)

        stdout = completed_process.stdout.decode('utf-8')
        stderr = completed_process.stderr.decode('utf-8')

        return_code = completed_process.returncode
        end_time = time.time()
        was_timeout = False

    except subprocess.TimeoutExpired:
        # This block will be entered if the command times out
        end_time = time.time()
        stdout = ''
        stderr = ''
        return_code = ''
        was_timeout = True

    except subprocess.CalledProcessError as e:
        # This block will be entered if the command fails
        end_time = time.time()
        stdout = e.stdout.decode('utf-8') if e.stdout else ''
        stderr = e.stderr.decode('utf-8') if e.stderr else ''
        return_code = e.returncode
        was_timeout = False

    except Exception as e:
        # This block will be entered if an unexpected exception occurs
        end_time = time.time()
        stdout = ''
        stderr = str(e)
        return_code = -1
        was_timeout = False

    return return_code, end_time - start_time, stdout, stderr, was_timeout


def extract_starting_numerical_prefix(filename):
    number = ''.join(filter(str.isdigit, filename))
    return int(number) if number else 0


def get_grouped_files(in_dir):
    groups = {}

    sorted_files = sorted(os.listdir(in_dir))
    sorted_files = [f for f in sorted_files if f.endswith(".in")]

    flatten = True

    for f in sorted_files:
        group = extract_starting_numerical_prefix(f)
        if group not in groups:
            groups[group] = [f]
        else:
            flatten = False
            groups[group].append(f)

    if flatten:
        return {'0': sorted_files}
    return groups


def create_parser():
    parser = argparse.ArgumentParser(description='Process some executables.')

    subparsers = parser.add_subparsers(dest='mode', required=True)

    parser_executable = subparsers.add_parser('exact')
    parser_executable.add_argument('executable', type=str, help='Path to the executable')
    parser_executable.add_argument('--time_limit', type=int, default=60, help='Time limit [sec] (default: 60)')
    parser_executable.add_argument('--max_time_limit_exceeded', type=int, default=10,
                                   help='Max time limit exceeded (default: 10)')

    parser_lb = subparsers.add_parser('lb')
    parser_lb.add_argument('executable', type=str, help='Path to the executable')
    parser_lb.add_argument('--time_limit', type=int, default=60, help='Time limit [sec] (default: 60)')
    parser_lb.add_argument('--max_time_limit_exceeded', type=int, default=10,
                           help='Max time limit exceeded (default: 10)')

    parser_ub = subparsers.add_parser('ub')
    parser_ub.add_argument('executable', type=str, help='Path to the executable')
    parser_ub.add_argument('--time_limit', type=int, default=60, help='Time limit [sec] (default: 60)')
    parser_ub.add_argument('--max_time_limit_exceeded', type=int, default=10,
                           help='Max time limit exceeded (default: 10)')

    parser_kernel = subparsers.add_parser('kernel')
    parser_kernel.add_argument('kernel_executable', type=str, help='Kernel executable file')
    parser_kernel.add_argument('heuristic_executable', type=str, help='Heuristic executable file')
    parser_kernel.add_argument('lifting_executable', type=str, help='Lifting executable file')
    parser.add_argument('--time_limit', type=int, default=60, help='Time limit [sec] (default: 60)')
    parser.add_argument('--max_time_limit_exceeded', type=int, default=1,
                        help='Max time limit exceeded (default: 1)')
    return parser


def main():

    global_score = 0

    parser = create_parser()

    args = parser.parse_args()
    mode = args.mode

    time_limit = args.time_limit
    max_time_limit_exceeded = args.max_time_limit_exceeded

    in_dir = "vc/in"
    out_dir = "vc/out"
    checker_file = "vc/chk.py"
    if mode == "kernel":
        print("file,status,points,time_kernel,time_lifting,return,stderr")
    elif mode == "lb" or mode == "ub":
        print("file,status,points,time,return,stderr")
    else:
        print("file,status,time,return,stderr")

    grouped_files = get_grouped_files(in_dir)
    found_error = False
    for group in grouped_files.values():
        tles = 0
        for f in group:
            in_file = os.path.join(in_dir, f)
            points = 0

            if mode == "kernel":
                stderr = ""
                return_code, time_kernel, stdout, stderr_kernel, was_timeout = run_command_with_limit(
                    args.kernel_executable,
                    in_file,
                    time_limit)
                time_kernel = "{:.3f}".format(time_kernel)

                stderr += stderr_kernel
                if not was_timeout and return_code == 0:
                    with open(".kernel.txt", 'w') as result:
                        result.write(stdout)

                    return_code, _, stdout, stderr_heu, was_timeout = run_command_with_limit(args.heuristic_executable,
                                                                                             ".kernel.txt",
                                                                                             60 * 1000 * 5)

                    stderr += stderr_heu

                    if not was_timeout and return_code == 0:
                        with open(".heuristic.txt", 'w') as result:
                            result.write(stdout)

                        with open(".lifting_in.txt", 'w') as result:
                            with open(in_file, 'r') as input:
                                with open(".heuristic.txt", 'r') as heuristic:
                                    with open(".kernel.txt", 'r') as kernel:
                                        result.write("#InputGraph\n")
                                        result.write(input.read())
                                        result.write("#KernelGraph\n")
                                        result.write(kernel.read())
                                        result.write("#StartSolution\n")
                                        result.write(heuristic.read())

                        return_code, time_lifting, stdout, stderr_lifting, was_timeout = run_command_with_limit(
                            args.lifting_executable, ".lifting_in.txt",
                            time_limit)

                        time_lifting = "{:.3f}".format(time_lifting)

                        stderr += stderr_lifting

                        if not was_timeout and return_code == 0:
                            with open(".lifting_out.txt", 'w') as result:
                                result.write(stdout)
                    else:
                        stderr += "Failed to run heuristic"
                        time_lifting = 0
                else:
                    time_lifting = 0



            else:
                return_code, time, stdout, stderr, was_timeout = run_command_with_limit(args.executable, in_file,
                                                                                        time_limit)
                time = "{:.3f}".format(time)

            if not was_timeout and return_code == 0:
                out_file = os.path.join(out_dir, f.replace(".in", ".out"))

                if os.path.exists(checker_file):
                    with open(".user_out.txt", 'w') as result:
                        result.write(stdout)
                    if mode == "kernel":
                        result = subprocess.run(
                            ["python3", checker_file, mode, in_file, ".lifting_out.txt", out_file, ".heuristic.txt",
                             ".kernel.txt"],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    else:
                        result = subprocess.run(["python3", checker_file, mode, in_file, ".user_out.txt", out_file],
                                                stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    stdout = result.stdout.decode('utf-8')
                    if "OK" in stdout:
                        status = "OK"
                        if mode == "kernel" or mode == "lb" or mode == "ub":
                            points = float(stdout.split("\n")[2].strip())
                    else:
                        status = "Wrong"
                        stderr += "\n" + stdout + "\n" + stderr
                else:
                    with open(out_file, 'r') as result:
                        solution = result.read()
                        if solution.strip() == stdout.strip():
                            status = "OK"
                        else:
                            status = "Wrong"
                            stderr += "\n" + "No checker" + "\n" + stderr

            elif was_timeout:
                status = "timelimit"
                time = ''
            else:
                stderr += "\nNon zero exit code"
                status = "Wrong"

            stderr = re.sub(r"\n", r"\\n", stderr)
            if mode == "kernel":
                print(f"{f},{status},{points},{time_kernel},{time_lifting},{return_code},{stderr}", flush=True)

                global_score += points # my code

            elif mode == "lb" or mode == "ub":
                print(f"{f},{status},{points},{time},{return_code},{stderr}", flush=True)

                global_score += points # my code

            else:
                print(f"{f},{status},{time},{return_code},{stderr}", flush=True)

            if status == "Wrong":
                found_error = True
                break
            if status == "timelimit":
                tles += 1
                if tles >= max_time_limit_exceeded:
                    break
        if found_error:
            raise Exception(f"Wrong Answer: {status}") # my code
            # break
    print(f"overall score: {global_score}")


if __name__ == '__main__':
    main()
