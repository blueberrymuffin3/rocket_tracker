import csv
import pathlib

manufacturing_path = pathlib.Path(__file__).parent.parent.absolute() / "Manufacturing"
pos_path = manufacturing_path / "rocket-bottom-pos.csv"
cpl_path = manufacturing_path / "cpl_JLCPCB.csv"

with pos_path.open("r") as pos_file, cpl_path.open("w") as cpl_file:
    pos_in = csv.reader(pos_file)
    next(pos_in) # ignore the header

    cpl_out = csv.writer(cpl_file, dialect="unix")
    cpl_out.writerow(["Designator", "Mid X", "Mid Y", "Layer", "Rotation"])

    total_positions = 0

    for position in pos_in:
        cpl_out.writerow([position[0], position[3] + "mm", position[4] + "mm", position[6].capitalize(), position[5]])
        total_positions += 1

    print(f"Wrote {total_positions} positions")
