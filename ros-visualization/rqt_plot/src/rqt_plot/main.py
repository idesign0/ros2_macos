import sys

from rqt_gui.main import Main
from rqt_plot.plot import Plot


def main():
    plugin = 'rqt_plot.plot.Plot'
    main = Main(filename=plugin)
    sys.exit(main.main(standalone=plugin, plugin_argument_provider=Plot.add_arguments))


if __name__ == '__main__':
    main()
