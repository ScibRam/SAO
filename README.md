# Seismicity Analysis Organizer
### Motivation
Nowadays plenty of tools available for seismologist: [SeisCode](https://seiscode.iris.washington.edu/), [ObsPy](https://github.com/obspy/obspy), [Madagascar](http://www.ahay.org/wiki/Main_Page) and more. Despite that, one may still want to use his own workflow, so basically here just a set of reinvented wheels for my PhD.

### Architecture
SAO allows you to send heavy processing routines on cluster or run locally and also easily plot data and results with nice graphic. Console UNIX-style C programs handle processing part. Wrapping Shell-scripts are used to set routines and parameters for execution these programs and could be also used as a jobs for SLURM. Finally Python modules are taking responsibility of data managing and plotting.

### Repository structures
* *kit* is for **toolkit** of changeable wrapping and routine scripts.
        Has environment dependency.
* *lib* is for **libraries** and header files, as well as for temporary files.
        Due to the license we don't care to separate them.
* *src* is naturally means **sourse** - core directory of the project.
        Subdirectories inside are organized by language/purpose pattern.
* *tst* is stands for **test** set to debug (dev/feature) or check (master).
        Each test is a shell script with name {scenario}.tst
* One *Makefile* to rule them all.


### License
Seismicity Analysis Organizer is licensed under the [GNU Lesser General Public License (LGPL) v3.0](https://www.gnu.org/copyleft/lesser.html)
