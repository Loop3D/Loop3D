import LoopProjectFile
from LoopStructural import GeologicalModel
from LoopStructural.visualisation import LavaVuModelViewer
import pandas
import numpy
from pyamg import solve

if ('loopFilename' not in vars() and 'loopFilename' not in globals()):
    loopFilename = "hammersley.loop3d"
else:
    print(loopFilename)

boundaries = [500057,603028,7455348,7567953,-1200,8200]
stepsizes = [1000,1000,300]
resp = LoopProjectFile.Get(loopFilename,"extents")
if resp["errorFlag"]: print(resp["errorString"])
else:
    boundaries = resp["value"]["utm"][2:] + resp["value"]["depth"]
    stepsizes = resp["value"]["spacing"]
#print(boundaries)
#print(stepsizes)

resp = LoopProjectFile.Get(loopFilename,"orientations")
if resp["errorFlag"]: print(resp["errorString"])
else: orientations = resp["value"]
orientationsPd = pandas.DataFrame(list(orientations),columns=["point","dipdir","dip","polarity","formation","layer"])
points=pandas.DataFrame(orientationsPd['point'].tolist(),columns=["X","Y","Z"])
orientationsPd = pandas.concat([points,orientationsPd.drop('point',1)],axis=1)
# print(orientationsPd)

resp = LoopProjectFile.Get(loopFilename,"stratigraphicLog")
if resp["errorFlag"]: print(resp["errorString"])
else: thickness = resp["value"]
thicknessPd = pandas.DataFrame(list(thickness),columns=["formation","thickness"])
thicknessDict = dict(thickness)
# print(thicknessPd)
# print(dict(thickness))

resp = LoopProjectFile.Get(loopFilename,"contacts")
if resp["errorFlag"]: print(resp["errorString"])
else: contacts = resp["value"]
contactsPd = pandas.DataFrame(list(contacts),columns=["point","formation"])
points=pandas.DataFrame(contactsPd['point'].tolist(),columns=["X","Y","Z"])
contactsPd = pandas.concat([points,contactsPd.drop('point',1)],axis=1)
# print(contactsPd)
# print("\n\n\n",contactsPd["formation"][1])

order = ['P__TKa_xs_k','P__TKo_stq','P__TKk_sf','P__TK_s',
  'A_HAu_xsl_ci', 'A_HAd_kd', 'A_HAm_cib', 'A_FOj_xs_b',
  'A_FO_xo_a', 'A_FO_od', 'A_FOu_bbo',
  'A_FOp_bs', 'A_FOo_bbo', 'A_FOh_xs_f', 'A_FOr_b']

strat_val = {}
val = 0
for o in order:
    if o in list(thicknessPd["formation"]):
        strat_val[o] = val
        val+=thicknessDict[o]
# print(strat_val)

contactsPd['val'] = numpy.nan

for o in strat_val:
    contactsPd.loc[contactsPd['formation']==o,'val'] = strat_val[o]
# print(contactsPd)
data = pandas.concat([orientationsPd,contactsPd],sort=False)
data['type'] = numpy.nan
for o in order:
    data.loc[data['formation']==o,'type'] = 's0'
# print(data)

# points=pandas.DataFrame(data['point'].tolist(),columns=["X","Y","Z"])
# print(points.shape)
# print(data.drop('point',1).shape)
# print(pandas.concat([points,data.drop('point',1)],axis=0))
boundary_points = numpy.zeros((2,3))
boundary_points[0,0] = boundaries[0]
boundary_points[0,1] = boundaries[2]
boundary_points[0,2] = boundaries[4]
boundary_points[1,0] = boundaries[1]
boundary_points[1,1] = boundaries[3]
boundary_points[1,2] = boundaries[5]

# Get resulting model for Loop interface
xstepsize = stepsizes[0]
ystepsize = stepsizes[1]
zstepsize = stepsizes[2]
xsteps = int((boundary_points[1,0]-boundary_points[0,0]) / xstepsize)+1
ysteps = int((boundary_points[1,1]-boundary_points[0,1]) / ystepsize)+1
zsteps = int((boundary_points[1,2]-boundary_points[0,2]) / zstepsize)+1

data = data[data.X >= boundary_points[0,0]]
data = data[data.Y >= boundary_points[0,1]]
data = data[data.Z >= boundary_points[0,2]]
data = data[data.X <= boundary_points[1,0]]
data = data[data.Y <= boundary_points[1,1]]
data = data[data.Z <= boundary_points[1,2]]

model = GeologicalModel(boundary_points[0,:],boundary_points[1,:],nsteps=(xsteps,ysteps,zsteps))
model.set_model_data(data)
strati = model.create_and_add_foliation('s0', #identifier in data frame
                                                    interpolatortype="PLI", #which interpolator to use
                                                    nelements=400000, # how many tetras/voxels
                                                    buffer=0.1, # how much to extend nterpolation around box
                                                    solver='pyamg',
                                                   )

#viewer = LavaVuModelViewer(model)
#viewer.add_data(strati['feature'])
#viewer.add_isosurface(strati['feature'])
##viewer.add_scalar_field(strati['feature'])
#viewer.set_viewer_rotation([-53.8190803527832, -17.1993350982666, -2.1576387882232666])
#viewer.save("fdi_surfaces.png")
#viewer.interactive()

xcoords = numpy.linspace(model.bounding_box[0, 0], model.bounding_box[1, 0], model.nsteps[0])
ycoords = numpy.linspace(model.bounding_box[0, 1], model.bounding_box[1, 1], model.nsteps[1])
zcoords = numpy.linspace(model.bounding_box[1, 2], model.bounding_box[0, 2], model.nsteps[2])
zz,xx,yy = numpy.meshgrid(zcoords,xcoords,ycoords,indexing='ij')
locs = numpy.transpose([xx.flatten(),yy.flatten(),zz.flatten()])
result = strati['feature'].evaluate_value(locs)

LoopProjectFile.Set(loopFilename,"strModel",data=numpy.reshape(result,(xsteps,ysteps,zsteps)),verbose=False)

import winsound
duration = 200  # milliseconds
freq = 1100  # Hz
winsound.Beep(freq, duration)
winsound.Beep(freq, duration)
winsound.Beep(freq, duration)