#!/usr/bin/env python
# coding: utf-8

# # Quickstart: Modelling a surface using geological observations
# This tutorial will demonstrate how to create an implicit surface representation of surfaces from a combination of orientation and location observations. 
# 
# Implicit surface representation involves finding an unknown function where $f(x,y,z)$ matches observations of the surface geometry. We generate a scalar field where the scalar value is the distance away from a reference horizon. The reference horizon is arbritary and can either be:
# 
#  * a single geological surface where the scalar field would represent the signed distance away from this surface. (above the surface positive and below negative)
#  * Where multiple conformable horizons are observed the same scalar field can be used to represent these surfaces and the thickness of the layers is used to determine the relative scalar value for each surface
# 
# 
# This tutorial will demonstrate both of these approaches for modelling a number of horizons picked from seismic data sets.

from LoopStructural.interpolators.piecewiselinear_interpolator import PiecewiseLinearInterpolator as PLI
from LoopStructural.supports.tet_mesh import TetMesh
from LoopStructural.modelling.features.geological_feature import GeologicalFeatureInterpolator
from LoopStructural.visualisation.model_visualisation import LavaVuModelViewer

import numpy as np
import lavavu
import matplotlib.pyplot as plt
import pandas as pd
import glob
#get_ipython().run_line_magic('matplotlib', 'inline')


# ### Importing data
# There are numerous ways to import data into python for LoopStructural. For this example there are three types of files: 
# 
# * `Dips.csv` which are the orientation observations X,Y,Z,XOrient,YOrient,ZOrient
# * `_Points.csv` which are point observations of the contacts X,Y,Z,Strati
# * `_Section.csv` which are section interpetations of the contacts X,Y,Z,Strati
# 
# The files can be imported into pandas dataframes. Three dataframes are created one for each of the observations types. 

dips = pd.read_csv('data/Dips.csv',delimiter=';')


# import all of the csv into the same dataframe use glob to find all files matching pattern
dfs = []
for f in glob.glob('data/*Points.csv'):
    dfs.append(pd.read_csv(f,delimiter=';'))
points = pd.concat(dfs,axis=0,ignore_index=True)

dfs = []
for f in glob.glob('data/*Section.csv'):
    dfs.append(pd.read_csv(f,delimiter=';'))
sections = pd.concat(dfs,axis=0,ignore_index=True)


# ### Setting up the interpolator
# In this example we use a discrete interpolator where the implicit function is approximated using a tetrahedral mesh. The mesh needs to be built that is the same size or larger than the area of interest. 
# We build the mesh by specifying the extent of the boundary box and the number of tetrahedron. 
# 
# An interpolator can be linked to this mesh.  

boundary_points = np.zeros((2,3))
boundary_points[0,0] = 548800
boundary_points[0,1] = 7816600
boundary_points[0,2] = -11010
boundary_points[1,0] = 552500
boundary_points[1,1] = 7822000
boundary_points[1,2] = -8400

# build the mesh
mesh = TetMesh()
mesh.setup_mesh(boundary_points, n_tetra=60000,)

# link mesh to the interpolator 
interpolator = PLI(mesh)


# ### Geological Features
# 
# A geological feature is any object that can be represented by a scalar field. This includes fold structural observations, stratigraphic groups of conformable layers, fault surfaces, unconformities etc.
# 
# In the next section the GeologicalFeatureInterpolator is used to build a geological feature. The abstraction of the geological feature away from the raw data or interpolator allows for easy exchange of interpolators and addition of geological structures such as faults and folds to an existing feature.
# 
# A data point can be added by calling the `add_point(position,value)` function or `add_planar_constraint(position, vector)`  

stratigraphy_builder = GeologicalFeatureInterpolator(
    interpolator=interpolator,
    name='stratigraphy')
solver = 'lu'
for i, r in points.iterrows():
    stratigraphy_builder.add_point([r['X'],r['Y'],r['Z']],r['Strati'])#xy[0][0],xy[1][0],z],r['value'],itype=r['itype'])
for i, r in sections.iterrows():
    stratigraphy_builder.add_point([r['X'],r['Y'],r['Z']],r['Strati'])#xy[0][0],xy[1][0],z],r['value'],itype=r['itype'])
# for i, r in dips.iterrows():
#     stratigraphy_builder.add_planar_constraint([r['X'],r['Y'],r['Z']],[r['OrientX'],r['OrientY'],r['OrientZ']])

    


# ### Run the interpolation algorithm
# The interpolation algorithm can be run by calling the build function for the stratigraphy_builder.
# 
# The default options can be used or arguments can be passed to the interpolator such as the algorithm to use for solving the least squares problem or the weighting of different elements.
# 
# In most cases the default options will be acceptable, however in some cases it may be beneficial to weight the values of the regularisation terms to be higher.
# 
# The default weights for DSI are `cgw=0.1`(the constant gradient regularisation), `cpw=1.0` (value control points) and `gpw=1.0` for the gradient control points.  
# 
# #### Choice of solver
# It is worthwhile noting that the choice of solver can impact the resulting solution and in some cases particular solvers will be unable to actually solve the system.
# For more information on the choice of solver read the tutorial on discrete implicit modelling. The default solver is the iterative conjugate gradient method implemented in scipy sparse. Other possible choices are `lu` which calls the lower upper decomposition method from scipy sparse. 

feature = stratigraphy_builder.build(cpw=1,cgw=0.1)

xstepsize = (boundary_points[1,0]-boundary_points[0,0]) / xsteps
xcoords = np.arange(boundary_points[0,0], boundary_points[1,0]+0.1*xstepsize,xstepsize)
ystepsize = (boundary_points[1,1]-boundary_points[0,1]) / ysteps
ycoords = np.arange(boundary_points[0,1], boundary_points[1,1]+0.1*ystepsize,ystepsize)
zstepsize = (boundary_points[1,2]-boundary_points[0,2]) / zsteps
zcoords = np.arange(boundary_points[0,2], boundary_points[1,2]+0.1*zstepsize,zstepsize)

zz,yy,xx = np.meshgrid(zcoords,ycoords,xcoords,indexing='ij')

locs = np.transpose([xx.flatten(),yy.flatten(),zz.flatten()])
result = feature.evaluate_value(locs)