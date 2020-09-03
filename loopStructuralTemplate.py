import LoopProjectFile
from LoopStructural import GeologicalModel
from LoopStructural.visualisation import LavaVuModelViewer
import pandas
import numpy
from pyamg import solve

# Check filename
if ('loopFilename' not in vars() and 'loopFilename' not in globals()):
    loopFilename = "hammersley.loop3d"
else:
    print("Opening loop project file: " + loopFilename)

# UTM boundary is [minEasting, maxEasting, minNorthing, maxNorthing, minDepth, maxDepth]
boundaries = [500057,603028,7455348,7567953,-1200,8200]
stepsizes = [1000,1000,300]
resp = LoopProjectFile.Get(loopFilename,"extents")
if resp["errorFlag"]: print(resp["errorString"])
else:
    boundaries = resp["value"]["utm"][2:] + resp["value"]["depth"]
    stepsizes = resp["value"]["spacing"]

# Get the stratigraphic observations (type is a byte array so convert to python string)
resp = LoopProjectFile.Get(loopFilename,"stratigraphicObservations")
if resp["errorFlag"]: print(resp["errorString"])
else: stratigraphicObservations = resp["value"]
stratObsPd = pandas.DataFrame(list(tuple(a) for a in stratigraphicObservations),columns=['layerId','X','Y','Z','dipDir','dip','polarity','type'])
stratObsPd['type'] = list(a.decode('ascii') for a in stratObsPd['type'])

# Get the stratigraphic log (formation is a byte array so convert to python string)
resp = LoopProjectFile.Get(loopFilename,"stratigraphicLog")
if resp["errorFlag"]: print(resp["errorString"])
else: stratigraphicLog = resp["value"]
stratLog = pandas.DataFrame(list(tuple(a) for a in stratigraphicLog),columns=\
    ['layerId','minAge','maxAge','formation','avgDisplacement','R0','G0','B0','R1','G1','B1'])
stratLog['formation'] = list(a.decode('ascii') for a in stratLog['formation'])

# Get the contacts and merge with the stratigraphic log
resp = LoopProjectFile.Get(loopFilename,"contacts")
if resp["errorFlag"]: print(resp["errorString"])
else: contacts = resp["value"]
contactsPd = pandas.DataFrame(list(tuple(a) for a in contacts),columns=['layerId','X','Y','Z'])
mergedPd = pandas.merge(stratLog,contactsPd,on=['layerId']) 

# Create an order to use (this should come out of the age of the layers from map2loop in the end)
order = ['P__TKa_xs_k','P__TKo_stq','P__TKk_sf','P__TK_s',
  'A_HAu_xsl_ci', 'A_HAd_kd', 'A_HAm_cib', 'A_FOj_xs_b',
  'A_FO_xo_a', 'A_FO_od', 'A_FOu_bbo',
  'A_FOp_bs', 'A_FOo_bbo', 'A_FOh_xs_f', 'A_FOr_b']

# Create a layer of contacts based on stratigraphic layer depths
strat_val = {}
val = 0
for o in order:
    if o in list(stratLog["formation"]):
        strat_val[o] = val
        val+=int(stratLog[stratLog['formation']==o]['avgDisplacement'])

# Assign the val field based on the stratigraphic depths
for o in strat_val:
    mergedPd.loc[mergedPd['formation']==o,'val'] = strat_val[o]
mergedPd['type'] = numpy.nan

# Assign to basement the entire stratigraphic log that is present in the order
for o in order:
    mergedPd.loc[mergedPd['formation']==o,'type'] = 's0'
data = pandas.concat([stratObsPd,mergedPd],sort=False)

# Convert boundaries to the format loopStructural uses
boundary_points = numpy.zeros((2,3))
boundary_points[0,0] = boundaries[0]
boundary_points[0,1] = boundaries[2]
boundary_points[0,2] = boundaries[4]
boundary_points[1,0] = boundaries[1]
boundary_points[1,1] = boundaries[3]
boundary_points[1,2] = boundaries[5]

xsteps = int((boundary_points[1,0]-boundary_points[0,0]) / stepsizes[0])+1
ysteps = int((boundary_points[1,1]-boundary_points[0,1]) / stepsizes[1])+1
zsteps = int((boundary_points[1,2]-boundary_points[0,2]) / stepsizes[2])+1

# Filter data of points outside of boundaries
data = data[data.X >= boundary_points[0,0]]
data = data[data.Y >= boundary_points[0,1]]
data = data[data.Z >= boundary_points[0,2]]
data = data[data.X <= boundary_points[1,0]]
data = data[data.Y <= boundary_points[1,1]]
data = data[data.Z <= boundary_points[1,2]]

# Create Loop Structural model
model = GeologicalModel(boundary_points[0,:],boundary_points[1,:],nsteps=(xsteps,ysteps,zsteps))
model.set_model_data(data)
strati = model.create_and_add_foliation('s0', #identifier in data frame
                                                    interpolatortype="PLI", #which interpolator to use
                                                    nelements=400000, # how many tetras/voxels
                                                    buffer=0.05, # how much to extend nterpolation around box
                                                    solver='pyamg',
                                                   )

# For debug can uncomment this to view data in LavaVu if that is installed
# Note that this viewer shows the depth inverted
# viewer = LavaVuModelViewer(model)
# viewer.add_data(strati['feature'])
# viewer.add_isosurface(strati['feature'])
# viewer.add_scalar_field(strati['feature'])
# viewer.set_viewer_rotation([-53.8190803527832, -17.1993350982666, -2.1576387882232666])
# viewer.save("fdi_surfaces.png")
# viewer.interactive()

# Get scalar field results and send to loop project file
xcoords = numpy.linspace(model.bounding_box[0, 0], model.bounding_box[1, 0], model.nsteps[0])
ycoords = numpy.linspace(model.bounding_box[0, 1], model.bounding_box[1, 1], model.nsteps[1])
zcoords = numpy.linspace(model.bounding_box[1, 2], model.bounding_box[0, 2], model.nsteps[2])
zz,yy,xx = numpy.meshgrid(zcoords,ycoords,xcoords,indexing='ij')
locs = numpy.transpose([xx.flatten(),yy.flatten(),zz.flatten()])
result = strati['feature'].evaluate_value(locs)
resp = LoopProjectFile.Set(loopFilename,"strModel",data=numpy.reshape(result,(xsteps,ysteps,zsteps)),verbose=False)

# Let me know when it's all done (loudly)
import winsound
duration = 200  # milliseconds
freq = 1100  # Hz
winsound.Beep(freq, duration)
winsound.Beep(freq, duration)
winsound.Beep(freq, duration)