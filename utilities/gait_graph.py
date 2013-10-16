#!/usr/bin/env python2.7
import sys, getopt
import argparse
import matplotlib.pyplot as plt

def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("file", help="file with the input data")
    parser.add_argument("threshold", help="value threshold for ground contact", type=float)
    parser.add_argument("-s", "--steps", type=int, help="maximum number of steps to plot")
    parser.add_argument("-g", "--grid", action="store_true", help="show a grid")
    parser.add_argument("-t", "--title", help="title of the graph")
    parser.add_argument("-o", "--output", help="save graph to specified file (SVG)")
    parser.add_argument("-v", "--verbosity", action="store_true", help="increase output verbosity")
    args = parser.parse_args()

    in_file = args.file
    legs = []
    threshold = args.threshold

    if args.verbosity:
        print 'gait_graph.py ...'
        print '-- Using input data file', in_file

    with open(in_file, "r") as f:
        in_data = f.read()
    f.close()

    lines = in_data.splitlines(False)

    # Parse leg names
    for line in lines:
        if line.startswith("#"):
            legs.append(line.split(":")[1])
        else:
            break;

    if args.verbosity:
        print '-- Calculating graph data for following legs:'
        print legs

    # parse values
    v = []
    for line in lines[len(legs)+1:]:
        if len(line) > 0 and not line.isspace():
            v.append(map(float, line.split(";")))

    vals = zip(*v)

    if args.verbosity:
        print '-- Number of values (time steps):', len(vals[0])
        print '-- Preparing data for plotting ...'

    # extract bar ranges for the graph
    ranges = [[] for _ in range(len(vals))]
    for i in range(len(vals)):
        contact = False
        for j in range(len(vals[i])):
            if j == args.steps:
                break
            val = vals[i][j]
            if not contact:
                if (threshold > 0 and val >= threshold) or (threshold < 0 and val <= threshold):
                    #print "Contact found at", j
                    cstart = j
                    contact = True
            else:
                if (threshold > 0 and val < threshold) or (threshold < 0 and val > threshold) or (j == len(vals[i])):
                    #print "Contact ended at", j
                    contact = False
                    ranges[i].append((cstart, j-cstart+1))

    # draw the graph
    fig, ax = plt.subplots()

    for i in range(len(ranges)):
        ax.broken_barh(ranges[i], (10*(i+1), 6), facecolors='black')

    ax.set_ylim(5,10*(len(ranges)+1)+1)
    ax.set_xlim(0,min(len(vals[0]),args.steps))
    ax.set_xlabel('time steps')
    ax.set_yticks([10*(i+1)+3 for i in range(len(legs))])
    ax.set_yticklabels(legs)
    ax.grid(args.grid)
    if args.title is not None:
        ax.set_title(args.title)

    if args.output is not None:
        #out_file = in_file.replace('.txt', '.svg')
        plt.savefig(args.output, format='svg')
        if args.verbosity:
            print '-- Generated graph saved to file', out_file

    plt.show()

if __name__ == '__main__':
	sys.exit(main())
