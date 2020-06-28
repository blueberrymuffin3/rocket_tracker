"""
    @package
    Generates two csv files (one for JLCPCB and one for DIGIKEY)
"""

# Import the KiCad python helper module and the csv formatter
import kicad_netlist_reader
import csv
import sys
import pathlib
import collections

# Generate an instance of a generic netlist, and load the netlist tree from
# the command line option. If the file doesn't exist, execution will stop
net = kicad_netlist_reader.netlist(sys.argv[1])

file_path = pathlib.Path(sys.argv[2]).parent.absolute() / "Manufacturing"

letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

# Allows for multiple part numbers seperated by
# commas for one part, all are added to the BOM.


def split_part_number(part_numbers):
    return map(lambda part_number: part_number.strip(), part_numbers.split(","))


with open(file_path / "bom_JLCPCB.csv", "w") as jlcpcb_file, open(file_path / "bom_DIGIKEY.csv", "w") as digikey_file:
    jlcpcb_parts = 0
    jlcpcb_out = csv.writer(jlcpcb_file, dialect="unix")
    jlcpcb_out.writerow(["Comment", "Designator", "Footprint", "LCSC Part #"])

    digikey_parts = 0
    digikey_out = csv.writer(digikey_file, dialect="unix")
    digikey_out.writerow(["Customer Reference", "Digi-Key Part Number", "Quantity"])

    grouped = net.groupComponents()

    # components_dict[supplier][part number] = list of components
    components_dict = collections.defaultdict(lambda: collections.defaultdict(lambda: []))

    for group in grouped:
        for component in group:
            supplier = component.getField("Supplier")

            part_numbers = []

            if(supplier == "JLCPCB"):
                part_numbers = split_part_number(component.getField("JLCPCB #"))
            elif(supplier == "DIGIKEY"):
                part_numbers = split_part_number(component.getField("DIGIKEY #"))

            for part_number in part_numbers:
                components_dict[supplier][part_number] += [component]

    for supplier, parts in components_dict.items():
        for part_number, components in parts.items():
            if(supplier == "JLCPCB"):
                for component in components:
                    jlcpcb_out.writerow([component.getValue(), component.getRef(), component.getFootprint(), part_number])
                    jlcpcb_parts += 1

            elif(supplier == "DIGIKEY"):
                digikey_out.writerow([", ".join(map(lambda component: component.getRef(), components)), part_number, len(components)])
                digikey_parts += 1

            else:
                print(f"No or unknown supplier for {component.getRef()}: {component.getValue()}")

    print(f"Wrote {jlcpcb_parts} parts for JLCPCB")
    print(f"Wrote {digikey_parts} parts for DIGIKEY")


# # Open a file to write to, if the file cannot be opened output to stdout
# # instead
# try:
#     f = open(sys.argv[2], 'w')
# except IOError:
#     e = "Can't open output file for writing: " + sys.argv[2]
#     print(__file__, ":", e, sys.stderr)
#     f = sys.stdout


# # Create a new csv writer object to use as the output formatter
# out = csv.writer(f, lineterminator='\n', delimiter=',',
#                  quotechar='\"', quoting=csv.QUOTE_ALL)

# # Output a set of rows for a header providing general information
# out.writerow(['Source:', net.getSource()])
# out.writerow(['Date:', net.getDate()])
# out.writerow(['Tool:', net.getTool()])
# out.writerow(['Generator:', sys.argv[0]])
# out.writerow(['Component Count:', len(net.components)])
# out.writerow(['Ref', 'Qnty', 'Value', 'Cmp name',
#               'Footprint', 'Description', 'Vendor'])

# # Get all of the components in groups of matching parts + values
# # (see ky_generic_netlist_reader.py)
# grouped = net.groupComponents()

# # Output all of the component information
# for group in grouped:
#     refs = ""

#     # Add the reference of every component in the group and keep a reference
#     # to the component so that the other data can be filled in once per group
#     for component in group:
#         refs += component.getRef() + ", "
#         c = component

#     # Fill in the component groups common data
#     out.writerow([refs, len(group), c.getValue(), c.getPartName(), c.getFootprint(),
#                   c.getDescription(), c.getField("Vendor")])
