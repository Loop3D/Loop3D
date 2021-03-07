global currentProgress
global currentProgressText
currentProgress = 1.0
currentProgressText = "Loading modules"

# from LoopStructural import GeologicalModel
import LoopProjectFile
import threading

def findGlobalVariable(varName, defaultValue):
    if (varName not in vars() and varName not in globals()):
        globals()[varName] = defaultValue

currentProgress = 7.5
currentProgressText = "Loading local variables from GUI"

findGlobalVariable('loopFilename',"default.loop3d")
findGlobalVariable('m2lDataDir',"m2l_data/")
findGlobalVariable('useLavavu',False)

currentProgress = 10.0
currentProgressText = "Collating data from Loop Project File - " + loopFilename

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

resp = LoopProjectFile.Get(loopFilename,"structuralModelsConfig")
if not resp["errorFlag"]:
    fault_params = resp['value'][0]
    foliation_params = resp['value'][1]

def threadFunc(loopFilename, m2lDataDir, fault_params, foliation_params, useLavavu, boundaries, stepsizes):
    def printTime(message,seconds):
        timeDiffMin = int(seconds / 60)
        timeDiffSec = int(seconds - timeDiffMin * 60)
        print(str(message) + str(timeDiffMin) + " minutes and " + str(timeDiffSec) + " seconds")

    try:
        skip_faults = False

        global errors
        errors = ""
        global currentProgress
        global currentProgressText

        from LoopStructural.visualisation import LavaVuModelViewer
        from LoopStructural.modelling.features.geological_feature import GeologicalFeature
        from LoopStructural.utils import build_model, process_map2loop
        import LoopProjectFile
        import numpy
        import traceback
        import time

        global m2l_data
        currentProgress = 20.0
        currentProgressText = "LoopStructural process map2loop data dir"

        start = time.time()
        dictt = {'skip_faults':skip_faults,'fault_params':fault_params,'foliation_params':foliation_params}
        m2l_data = process_map2loop(m2lDataDir,dictt)
        postInit = time.time()
        printTime("LOOPSTRUCTURAL process_map2loop took ", postInit-start)

        currentProgress = 35.0
        currentProgressText = "LoopStructural building model"

        model = build_model(m2l_data,
        # model, m2l_data = GeologicalModel.from_map2loop_directory(m2lDataDir,
            skip_faults=skip_faults,
            fault_params=fault_params,
            foliation_params=foliation_params
            )
        postModel = time.time()
        printTime("LOOPSTRUCTURAL build_model took ", postModel-postInit)

        currentProgress = 65.0
        currentProgressText = "LoopStructural showing model with Lavavu"

        if (useLavavu):
            view = LavaVuModelViewer(model,vertical_exaggeration=1)
            view.nsteps = numpy.array([200,200,200])
            view.add_model(cmap='tab20')
            view.nsteps=numpy.array([50,50,50])
            view.add_model_surfaces()
            view.add_isosurface(model.get_feature_by_name("supergroup_0"),nslices=5)
            view.interactive()
            postLavavu = time.time()
            printTime("LOOPSTRUCTURAL lavavu took ", postLavavu-postModel)

        currentProgress = 75.0
        currentProgressText = "LoopStructural evaluating 3D structure for storage"

        preEval = time.time()
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


        ran = range(len(geologicalFeatureNames))

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

        postEval = time.time()
        printTime("LOOPSTRUCTURAL evaluate_value took ", postEval-preEval)
        printTime("LOOPSTRUCTURAL total calcs took ", postEval-start)

        currentProgress = 95.0
        currentProgressText = "LoopStructural updating loop project file"

        resp = LoopProjectFile.Set(loopFilename,"strModel",data=numpy.reshape(result,(xsteps,ysteps,zsteps)),verbose=False)

        time.sleep(1)
        currentProgress = 0.0
        currentProgressText = "LoopStructural Completed"
    
    except Exception as e:
        errors += "PythonError: \n" + traceback.format_exc() + '\n' + repr(e)
        print(errors)
        currentProgress = 666.0
        currentProgressText = "LoopStructural exception Thrown:" + errors
    
x = threading.Thread(target=threadFunc,
    args=[loopFilename,
          m2lDataDir,
          fault_params,
          foliation_params,
          useLavavu,
          boundaries,
          stepsizes
          ])

try:
    x.start()
except Exception as e:
    print(e)
