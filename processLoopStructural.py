global currentProgress
global currentProgressText
currentProgress = 1.0
currentProgressText = "Loading modules"

import LoopProjectFile
import threading

skip_faults = False

def findGlobalVariable(varName, defaultValue):
    if (varName not in vars() and varName not in globals()):
        globals()[varName] = defaultValue

currentProgress = 7.5
currentProgressText = "Loading local variables from GUI"

findGlobalVariable("loopFilename","default.loop3d")
findGlobalVariable("m2lDataDir","m2l_data")
findGlobalVariable("useLavavu",False)

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
        import pandas
        import time
        import math

        currentProgress = 20.0
        currentProgressText = "LoopStructural loading more data from loop project file"

        start = time.time()
        dictt = {'skip_faults':skip_faults,'fault_params':fault_params,'foliation_params':foliation_params}

        resp = LoopProjectFile.Get(loopFilename,"contacts")
        if resp["errorFlag"]:
            print(resp["errorString"])
        else:
            contacts = pandas.DataFrame.from_records(resp["value"],
                columns=['eventId','X','Y','Z','type'])

        resp = LoopProjectFile.Get(loopFilename,"foliationObservations")
        if resp["errorFlag"]:
            print(resp["errorString"])
        else:
            folObs = pandas.DataFrame.from_records(resp["value"],
                columns=['eventId','X','Y','Z','type','dipDir','dip'])

        resp = LoopProjectFile.Get(loopFilename,"faultObservations")
        if resp["errorFlag"]:
            print(resp["errorString"])
        else:
            faultObs = pandas.DataFrame.from_records(resp["value"],
                columns=['eventId','X','Y','Z','type','dipDir','dip','dipPol',
                         'val','displacement','posOnly'])

        resp = LoopProjectFile.Get(loopFilename,"faultLog")
        if resp["errorFlag"]:
            print(resp["errorString"])
        else:
            faultEvents = pandas.DataFrame.from_records(resp["value"],
                columns=['eventId','minAge','maxAge','name','supergroup','enabled',
                         'rank','type','avgDisplacement','avgDownthrowDir',
                         'influenceDistance','verticalRadius','horizontalRadius','colour'])
            faultEvents['name'] = faultEvents['name'].str.decode('utf-8')
            faultEvents['supergroup'] = faultEvents['supergroup'].str.decode('utf-8')
            faultEvents['colour'] = faultEvents['colour'].str.decode('utf-8')

        resp = LoopProjectFile.Get(loopFilename,"foliationLog")
        if resp["errorFlag"]:
            print(resp["errorString"])
        else:
            foliationEvents = pandas.DataFrame.from_records(resp["value"],
                columns=['eventId','minAge','maxAge','name','supergroup','enabled',
                         'rank','type','lowerScalarValue','upperScalarValue'])
            foliationEvents['name'] = foliationEvents['name'].str.decode('utf-8')
            foliationEvents['supergroup'] = foliationEvents['supergroup'].str.decode('utf-8')

        resp = LoopProjectFile.Get(loopFilename,"foldLog")
        if resp["errorFlag"]:
            print(resp["errorString"])
        else:
            foldEvents = pandas.DataFrame.from_records(resp["value"],
                columns=['eventId','minAge','maxAge','name','supergroup','enabled',
                         'rank','type','periodic','wavelength','amplitude',
                         'asymmetry','asymmetryShift','secondaryWavelength',
                         'secondaryAmplitude'])
            foldEvents['name'] = foldEvents['name'].str.decode('utf-8')
            foldEvents['supergroup'] = foldEvents['supergroup'].str.decode('utf-8')

        resp = LoopProjectFile.Get(loopFilename,"stratigraphicLog")
        if resp["errorFlag"]:
            print(resp["errorString"])
        else:
            stratEvents = pandas.DataFrame.from_records(resp["value"],
                columns=['eventId','minAge','maxAge','name','supergroup','enabled',
                         'rank','type','thickness',
                         'colour1Red','colour1Green','colour1Blue',
                         'colour2Red','colour2Green','colour2Blue'])
            stratEvents['name'] = stratEvents['name'].str.decode('utf-8')
            stratEvents['supergroup'] = stratEvents['supergroup'].str.decode('utf-8')

        resp = LoopProjectFile.Get(loopFilename,"stratigraphicObservations")
        if resp["errorFlag"]:
            print(resp["errorString"])
        else:
            stratObs = pandas.DataFrame.from_records(resp["value"],
                columns=['eventId','X','Y','Z','type','dipDir','dip','polarity',
                         'layer'])
            stratObs['layer'] = stratObs['layer'].str.decode('utf-8')

        data = stratObs.merge(stratEvents[['eventId','name','supergroup']],on='eventId')
        data.rename(columns={'name':'formation','supergroup':'group'},inplace=True)
        data['tz'] = 0
        data['tx'] = numpy.sin(numpy.deg2rad(data['dipDir']-90))
        data['ty'] = numpy.cos(numpy.deg2rad(data['dipDir']-90))
        data.drop(columns=['type','dipDir','dip','eventId'],inplace=True)
        data['feature_name'] = data['group']

        def getNormalFromDipDir(dipDir,dip):
            vec3 = (numpy.sin(numpy.deg2rad(dip))*numpy.cos(numpy.deg2rad(dipDir)),numpy.sin(numpy.deg2rad(dip))*numpy.sin(numpy.deg2rad(dipDir)),numpy.cos(numpy.deg2rad(dip)))
            return vec3

        data2 = stratObs.merge(stratEvents[['eventId','name','supergroup','thickness']],on='eventId')
        data2.rename(columns={'name':'formation','supergroup':'feature_name'},inplace=True)
        cartesian = getNormalFromDipDir(data2['dipDir'],data2['dip'])
        data2['nx'] = cartesian[0] * data2['thickness']
        data2['ny'] = cartesian[1] * data2['thickness']
        data2['nz'] = cartesian[2] * data2['thickness']
        data2.drop(columns=['type','dipDir','dip','eventId','thickness'],inplace=True)

        stratEvents['accumAlt'] = numpy.cumsum(stratEvents['thickness']) * -1
        data3 = contacts.merge(stratEvents[['eventId','name','supergroup','accumAlt']],on='eventId')
        data3.rename(columns={'name':'formation','supergroup':'feature_name','accumAlt':'val'},inplace=True)
        data3['index'] = data3.index
        data3.drop(columns=['type','eventId'],inplace=True)

        data2['feature_name'] = "supergroup_0"
        data3['feature_name'] = "supergroup_0"

        def sphericalPolarToXYZ(psi, theta):
            return (numpy.sin(numpy.deg2rad(theta)) * numpy.cos(numpy.deg2rad(psi)),
                    numpy.sin(numpy.deg2rad(theta)) * numpy.sin(numpy.deg2rad(psi)),
                    numpy.cos(numpy.deg2rad(theta)))

        stratigraphicColumn = {}
        stratigraphicColumn['supergroup_0'] = {}
        for strata in stratEvents.itertuples():
            # Currently commented out as multiple supergroups currently not working
            # if not strata.supergroup in stratigraphicColumn:
                # stratigraphicColumn[strata.supergroup] = {}
            # stratigraphicColumn[strata.supergroup][strata.name] = {  
            stratigraphicColumn['supergroup_0'][strata.name] = {
                'max':strata.accumAlt + strata.thickness,
                'min': strata.accumAlt,
                'id': strata.eventId,
                'colour': str('#') + str(hex(strata.colour1Red))[2:4] + str(hex(strata.colour1Green))[2:4] + str(hex(strata.colour1Blue))[2:4]
                }
        data4 = pandas.DataFrame()
        completeFaults = faultEvents.merge(faultObs,on='eventId')
        stratigraphicColumn['faults'] = {}
        for fault in completeFaults.itertuples():
            if not math.isnan(fault.dipDir):
                stratigraphicColumn['faults'][fault.name] = {
                    'FaultCenter':numpy.array([fault.X,fault.Y,fault.Z]),
                    'FaultDipDirection':fault.dipDir,
                    'InfluenceDistance':numpy.array(fault.influenceDistance),
                    'HorizontalRadius':numpy.array(fault.horizontalRadius),
                    'VerticalRadius':numpy.array(fault.verticalRadius),
                    'colour':numpy.array(fault.colour)
                }
                vec3Dip = sphericalPolarToXYZ(fault.dipDir,fault.dip)
                data4 = data4.append({'X':fault.X,'Y':fault.Y,'Z':fault.Z,'formation':fault.name,'feature_name':fault.name,'coord':fault.posOnly*2,'gx':vec3Dip[0],'gy':vec3Dip[1],'gz':vec3Dip[2]}, ignore_index=True)
                faultCentreXYZP = numpy.array([fault.X,fault.Y,fault.Z,fault.dipDir])
                normalVector = numpy.array([numpy.sin(numpy.deg2rad(fault.dipDir)),numpy.cos(numpy.deg2rad(fault.dipDir)),0])
                strikeVector = numpy.array([normalVector[1],-normalVector[0],0])
                faultEdges = numpy.array(faultCentreXYZP[:3] + normalVector * fault.influenceDistance)
                data4 = data4.append({'X':faultEdges[0],'Y':faultEdges[1],'Z':faultEdges[2],'formation':fault.name,'feature_name':fault.name,'val':1,'coord':0},ignore_index=True)
                faultEdges = numpy.array(faultCentreXYZP[:3] - normalVector * fault.influenceDistance)
                data4 = data4.append({'X':faultEdges[0],'Y':faultEdges[1],'Z':faultEdges[2],'formation':fault.name,'feature_name':fault.name,'val':-1,'coord':0},ignore_index=True)
                faultTips = numpy.array(faultCentreXYZP[:3] + strikeVector * fault.horizontalRadius)
                data4 = data4.append({'X':faultTips[0],'Y':faultTips[1],'Z':faultTips[2],'formation':fault.name,'feature_name':fault.name,'val':1,'coord':2},ignore_index=True)
                faultTips = numpy.array(faultCentreXYZP[:3] - strikeVector * fault.horizontalRadius)
                data4 = data4.append({'X':faultTips[0],'Y':faultTips[1],'Z':faultTips[2],'formation':fault.name,'feature_name':fault.name,'val':-1,'coord':2},ignore_index=True)
            else:
                data4 = data4.append({'X':fault.X,'Y':fault.Y,'Z':fault.Z,'formation':fault.name,'feature_name':fault.name,'coord':fault.posOnly*2}, ignore_index=True)

        

        global m2l_data
        m2l_data = {}
        m2l_data['data'] = pandas.concat([data,data2,data3,data4])
        m2l_data['groups'] = stratEvents[['name','supergroup']]
        m2l_data['groups'].rename(columns={'name':'code','supergroup':'group'},inplace=True)
        m2l_data['groups']['group'] = "supergroup_0"
        count = 1
        groupName = m2l_data['groups']['group'][0]
        groupNumbers=[]
        for group in m2l_data['groups'].itertuples():
            if groupName != group.group:
                groupName = group.group
                count = count + 1
            groupNumbers.append(count)
        m2l_data['groups']['group number'] = groupNumbers
        m2l_data['max_displacement'] = faultEvents[['name','avgDisplacement']].set_index('name').to_dict()['avgDisplacement']
        m2l_data['fault_fault']= []
        m2l_data['stratigraphic_column'] = stratigraphicColumn
        m2l_data['bounding_box'] = pandas.DataFrame([boundaries],columns=['minx','maxx','miny','maxy','lower','upper'])
        m2l_data['strat_va'] = stratEvents[['name','accumAlt']].set_index('name').to_dict()['accumAlt']
        m2l_data['downthrow_dir'] = {}
        for fault in completeFaults.drop_duplicates().itertuples():
            m2l_data['downthrow_dir'][fault.name] = numpy.array([fault.avgDownthrowDir,fault.X,fault.Y])

        postInit = time.time()
        printTime("LOOPSTRUCTURAL collection data from project file took ", postInit-start)

        currentProgress = 35.0
        currentProgressText = "LoopStructural building model"

        global model
        model = build_model(m2l_data,
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
           maxvals.append(max+20.0)

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