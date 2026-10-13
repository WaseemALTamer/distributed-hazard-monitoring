from pathlib import Path
import argparse
import sys


current_dir = Path(__file__).resolve().parent
sys.path.insert(0, str(current_dir))



from simulator import Simulator


def main() -> None:
    parser = argparse.ArgumentParser(description="Broadcast virtual sensor nodes over UDP.")
    parser.add_argument("--normal", type=int, default=3, help="number of normal nodes (default: 3)")
    parser.add_argument("--dangerous", type=int, default=1, help="number of dangerous nodes (default: 1)")
    parser.add_argument("--interval", type=float, default=10.0, help="seconds between broadcasts (default: 10)")
    parser.add_argument("--port", type=int, default=39394, help="UDP destination port (default: 39394)")
    args = parser.parse_args()

    with Simulator(
        normal_nodes=args.normal,
        dangerous_nodes=args.dangerous,
        interval=args.interval,
        port=args.port,
    ) as simulator:
        simulator.run()


if __name__ == "__main__":
    main()