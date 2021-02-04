from LoopStructural import GeologicalModel
from LoopStructural.visualisation import LavaVuModelViewer
from LoopStructural.modelling.features.geological_feature import GeologicalFeature
from LoopStructural.utils import build_model, process_map2loop
import LoopProjectFile
import numpy
import traceback

skip_faults = False

def findGlobalVariable(varName, defaultValue):
    if (varName not in vars() and varName not in globals()):
        globals()[varName] = defaultValue

findGlobalVariable('useLavavu',False)
findGlobalVariable('m2lDataDir',"m2l_data/")
findGlobalVariable('loopFilename',"default.loop3d")

# UTM boundary is [minEasting, maxEasting, minNorthing, maxNorthing, minDepth, maxDepth]
resp = LoopProjectFile.Get(loopFilename,"extents")
if resp["errorFlag"]:
    boundaries = [500102.854, 603064.443,
                  7455392.3, 7567970.26, -12000.0, 1200.0]
    stepsizes = [1000,1000,300]
    print(resp["errorString"])
else:
    boundaries = resp["value"]["utm"][2:] + resp["value"]["depth"]
    stepsizes = resp["value"]["spacing"]

solver = 'pyamg'
# solver = 'cg'
# solver = 'lu'
fault_params = {'interpolatortype':'FDI',
    'nelements':3e4,
    'data_region':.3,
    'solver':solver,
    # overprints:overprints,
    'cpw':10,
    'npw':10}
foliation_params = {'interpolatortype':'PLI' , # 'interpolatortype':'PLI',
    'nelements':1e5,  # how many tetras/voxels
    'buffer':0.8,  # how much to extend nterpolation around box
    'solver':solver,
    'damp':True}

errors = ""
try:
    dictt = {'skip_faults':skip_faults,'fault_params':fault_params,'foliation_params':foliation_params}
    m2l_data = process_map2loop(m2lDataDir,dictt)
    model = build_model(m2l_data,
    # model, m2l_data = GeologicalModel.from_map2loop_directory(m2lDataDir,
        skip_faults=skip_faults,
        fault_params=fault_params,
        foliation_params=foliation_params
        )

    if (use_lavavu):
        view = LavaVuModelViewer(model,vertical_exaggeration=1) 
        view.nsteps = numpy.array([200,200,200])
        view.add_model(cmap='tab20')
        view.nsteps=numpy.array([50,50,50])
        view.add_model_surfaces()
        view.add_isosurface(model.get_feature_by_name("supergroup_0"),nslices=5)
        view.interactive()  

    xsteps = int((boundaries[1]-boundaries[0]) / stepsizes[0])+1
    ysteps = int((boundaries[3]-boundaries[2]) / stepsizes[1])+1
    zsteps = int((boundaries[5]-boundaries[4]) / stepsizes[2])+1
    # Get scalar field results and send to loop project file
    xcoords = numpy.linspace(model.bounding_box[0, 0], model.bounding_box[1, 0], xsteps)
    ycoords = numpy.linspace(model.bounding_box[0, 1], model.bounding_box[1, 1], ysteps)
    zcoords = numpy.linspace(model.bounding_box[0, 2], model.bounding_box[1, 2], zsteps)
    zz,yy,xx = numpy.meshgrid(zcoords,ycoords,xcoords,indexing='ij')
    locs = numpy.transpose([xx.flatten(),yy.flatten(),zz.flatten()])

    geologicalFeatureNames = []
    sfs = []
    count = 0
    for f in model.features:
        if type(f) == GeologicalFeature:
            sfs.append(f.evaluate_value(locs))
            geologicalFeatureNames.append(f.name)

    sfs2 = []
    for sf in sfs:
        sfs2.append(numpy.reshape(sf,(xsteps,ysteps,zsteps)))


    option = 0
    ran = range(len(geologicalFeatureNames))

    if option == 0:
        maxvals = [0]
        for i in range(len(geologicalFeatureNames)-1):
            max = -999999
            for j in range(len(sfs[0])):
                if sfs[i][j] < 0 and sfs[i+1][j] > max:
                    max = sfs[i+1][j]
            maxvals.append(max+2000.0)

        result = []
        val = sfs[-1]
        for j in range(len(sfs[0])):
            for i in reversed(ran):
                if sfs[i][j] > 0:
                    val[j] = sfs[i][j] + sum(maxvals[len(ran)-i:])
            result.append(val[j])

    if option == 1:
        result = []
        for i in range(len(sfs[0])):
            val = sfs[0][i]
            for j in ran:
                if val < 0:
                    val = sfs[j][i]
            result.append(val)


    if option == 2:
        result = sfs[0]

    resp = LoopProjectFile.Set(loopFilename,"strModel",data=numpy.reshape(result,(xsteps,ysteps,zsteps)),verbose=False)
except Exception as e:
    errors += "PythonError: \n" + traceback.format_exc() + '\n' + repr(e)


# Let me know when it's all done (loudly)
import winsound
duration = 200  # milliseconds
freq = 1100  # Hz
winsound.Beep(freq, duration)
winsound.Beep(freq, duration)
winsound.Beep(freq, duration)