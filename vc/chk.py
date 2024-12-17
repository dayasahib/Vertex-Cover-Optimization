import argparse
import sys
import re


def create_parser():
    parser = argparse.ArgumentParser(description='Process some executables.')

    subparsers = parser.add_subparsers(dest='mode')

    parser_executable = subparsers.add_parser('exact')
    parser_executable.add_argument('input_file', type=str)
    parser_executable.add_argument('user_output_file', type=str)
    parser_executable.add_argument('model_output_file', type=str)

    parser_lb = subparsers.add_parser('lb')
    parser_lb.add_argument('input_file', type=str)
    parser_lb.add_argument('user_output_file', type=str)
    parser_lb.add_argument('model_output_file', type=str)

    parser_ub = subparsers.add_parser('ub')
    parser_ub.add_argument('input_file', type=str)
    parser_ub.add_argument('user_output_file', type=str)
    parser_ub.add_argument('model_output_file', type=str)

    parser_kernel = subparsers.add_parser('kernel')
    parser_kernel.add_argument('input_file', type=str)
    parser_kernel.add_argument('user_output_file', type=str)
    parser_kernel.add_argument('model_output_file', type=str)
    parser_kernel.add_argument('heuristic_file', type=str)
    parser_kernel.add_argument('kernel_file', type=str)

    return parser


parser = create_parser()
args = parser.parse_args()


def read_solution_size(solution_file):
    with open(solution_file, 'r') as f:
        try:
            for line in f:
                if line.startswith("#"):
                    continue
                words = line.split()
                sol_size = int(words[0])
                return sol_size  # Return the size of the optimal solution
            raise ValueError
        except ValueError:  # If the conversion to integer fails
            print("Error: Can not read solution size from model output file")
            sys.exit(1)  # Exit the program


def read_lower_bound(filename):
    with open(filename, 'r') as f:
        pattern = r'#\s*lower_bound[:]?\s*(\d+)'
        match = re.search(pattern, f.read())
        if match:
            number = int(match.group(1))
            return number
        else:
            raise ValueError(f"Bad heu output. Report this issue in the forum. Thanks.")


def read_difference_budget(kernel_file):
    with open(kernel_file, 'r') as f:
        pattern = r'#\s*difference:\s*(\d+)'
        match = re.search(pattern, f.read())

        if match:
            number = int(match.group(1))
            return number
        else:
            raise ValueError(f"Unable to find difference")


def read_graph_info(file, check_correctness=True):
    n = None
    m = None
    nodes = set()
    edges = set()

    with open(file, 'r') as f:
        for line in f.readlines():
            line = line.split("#")[0].strip()
            if len(line) == 0:
                continue

            if n is None:
                n = int(line.split()[0])
                m = int(line.split()[1])
                if not check_correctness:
                    return (n, m)
            else:
                u = line.split()[0]
                v = line.split()[1]
                nodes.add(u)
                nodes.add(v)
                edges.add((u, v))
                edges.add((v, u))

    real_n = len(nodes)
    real_m = len(edges) // 2
    if check_correctness:
        if n < real_n or m < real_m:
            print("WRONG\nGraph has wrong nodes or edge information")
            sys.exit(1)
        return (n, m)

    return (None, None)


def read_solution(file):
    vc = set()
    with open(file, 'r') as f:
        for line in f.readlines():
            line = line.split("#")[0].strip()
            if line == '':
                continue  # Skip empty lines

            vc.add(line)
    return vc


def check_if_vc(vc_user):
    with open(args.input_file, 'r') as f:
        f.readline()
        for line in f.readlines():
            edge = tuple(line.split())
            if edge[0] not in vc_user and edge[1] not in vc_user:
                print("WRONG\nEdge not covered")
                sys.exit(1)


def check_kernel():
    real_sol_size = read_solution_size(args.model_output_file)

    n_original, m_original = read_graph_info(args.input_file, check_correctness=False)
    n_reduced, m_reduced = read_graph_info(args.kernel_file)

    try:
        d = read_difference_budget(args.kernel_file)
    except ValueError:
        print("WRONG\nCan not read difference size")
        sys.exit(1)

    if d > real_sol_size:
        print("WRONG\nDifference budget is too large")
        sys.exit(1)

    vc_heuristic = read_solution(args.heuristic_file)
    vc_user = read_solution(args.user_output_file)
    check_if_vc(vc_user)
    if len(vc_user) > len(vc_heuristic) + d:
        print("WRONG\nToo many nodes")
        sys.exit(1)

    try:
        gp_lb = read_lower_bound(args.heuristic_file)
    except ValueError:
        print("ERROR\nInconsistencty in script/heu. Please report issue in forum")
        sys.exit(1)

    if d + gp_lb > real_sol_size:
        print("WRONG\nGraph incorrectly reduced. Opt for kernel + difference > opt for original graph")
        sys.exit(1)

    score = max(0.0, 1 - (n_reduced + m_reduced) / (n_original + m_original))
    print(f"OK\nOK\n{score}")


def check_exact():
    sol_size = read_solution_size(args.model_output_file)
    vc = read_solution(args.user_output_file)

    check_if_vc(vc)

    if len(vc) > sol_size:
        print("WRONG\nToo many nodes")
        sys.exit(1)

    print("OK")


def check_ub():
    sol_size = read_solution_size(args.model_output_file)

    vc = read_solution(args.user_output_file)

    check_if_vc(vc)

    if len(vc) == 0:
        score = 1.0
    else:
        score = sol_size / len(vc)

    print(f"OK\nOK\n{score}")


def check_lb():
    sol_size = read_solution_size(args.model_output_file)
    lb = None
    with open(args.user_output_file, 'r') as f:
        for line in f.readlines():
            line = line.split("#")[0].strip()
            if line == '':
                continue  # Skip empty lines
            try:
                lb = int(line)
                break
            except ValueError:
                print("WRONG\nCan not read lower bound")
                sys.exit(1)

    if lb is None:
        print("WRONG\nCan not read lower bound")
        sys.exit(1)

    if lb > sol_size:
        print("WRONG\nLower bound is too large")
        sys.exit(1)

    if sol_size == 0:
        score = 1.0
    else:
        score = lb / sol_size
    print(f"OK\nOK\n{score}")


def main():
    mode = args.mode

    if mode == 'exact':
        check_exact()
    elif mode == 'kernel':
        check_kernel()
    elif mode == 'lb':
        check_lb()
    elif mode == 'ub':
        check_ub()


if __name__ == '__main__':
    main()
