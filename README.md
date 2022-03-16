# Julia Set Fractals
Command line program to generate fractal GIFs belonging to the julia sets with function z<sub>0</sub><sup>2</sup>+C for some constant complex number C, and z<sub>0</sub> in a given range.

## Building & Running

To install pip packages and build the C extension:  
```
python -m pip install -r requirements.txt
python setup.py install
```

To run:
```
python julia_set.py
```
