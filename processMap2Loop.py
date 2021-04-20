global currentProgress
global currentProgressText
currentProgress = 1.0
currentProgressText = "Loading modules"

import LoopProjectFile
import os
import threading
currentProgress = 5.0

def findGlobalVariable(varName, defaultValue):
    if (varName not in vars() and varName not in globals()):
        globals()[varName] = defaultValue

currentProgress = 7.5
currentProgressText = "Loading local variables from GUI"

# Accept loopFilename and m2lDataDir from python state otherwise default
findGlobalVariable("loopFilename","default.loop3d")
findGlobalVariable("m2lDataDir","m2l_data")
findGlobalVariable("m2lParams",{})
findGlobalVariable("m2lFiles",{})
findGlobalVariable("m2lQuietMode",'all')

if(not os.path.isdir("m2l_data")):
    os.mkdir("m2l_data")
if(not os.path.isdir(m2lDataDir)):
    os.mkdir(m2lDataDir)
if(not os.path.isdir(m2lDataDir + "/tmp")):
    os.mkdir(m2lDataDir + "/tmp")
if(not os.path.isdir(m2lDataDir + "/output")):
    os.mkdir(m2lDataDir + "/output")
if(not os.path.isdir(m2lDataDir + "/dtm")):
    os.mkdir(m2lDataDir + "/dtm")
# if(not os.path.isdir(m2lDataDir + "/vtk")):
    # os.mkdir(m2lDataDir + "/vtk")
if(not os.path.isdir(m2lDataDir + "/graph")):
    os.mkdir(m2lDataDir + "/graph")

# Set default values for boundaries and projection
proj_crs = {"init": "EPSG:28350"}
boundaries = [500102.854, 603064.443, 7455392.3, 7567970.26, -12000.0, 1200.0]

currentProgress = 10.0
currentProgressText = "Loading extents from project file"

# TODO: Add utm/gda zone when that can happen
# Collect extents from loopFilename
resp = LoopProjectFile.Get(loopFilename, "extents")
if resp["errorFlag"]:
    print(resp["errorString"])
else:
    boundaries = resp["value"]["utm"][2:] + resp["value"]["depth"]
    proj_crs = {"init": "EPSG:"+str(28300+resp["value"]["utm"][0])}

bbox = {"minx": boundaries[0], "maxx": boundaries[1], "miny": boundaries[2],
        "maxy": boundaries[3], "base": boundaries[4], "top": boundaries[5]}
bboxStr=str(bbox["minx"])+","+str(bbox["miny"])+","+str(bbox["maxx"])+","+str(bbox["maxy"])

currentProgress = 15.0
currentProgressText = "Starting Map2Loop Project initialisation"

def threadFunc(loopFilename, m2lDataDir, m2lFiles, m2lParams, m2lQuietMode, bbox, proj_crs):
    import time
    import traceback
    import pandas
    global errors
    errors = ""
    def findGlobalVariable(varName, defaultValue):
        if (varName not in vars() and varName not in globals()):
            globals()[varName] = defaultValue
    def printTime(message,seconds):
        timeDiffMin = int(seconds / 60)
        timeDiffSec = int(seconds - timeDiffMin * 60)
        print(str(message) + str(timeDiffMin) + " minutes and " + str(timeDiffSec) + " seconds")

    try:
        import map2loop
        from map2loop.project import Project
        global currentProgress
        global currentProgressText
        start = time.time()
        proj = Project(**m2lFiles)
        postInit = time.time()
        printTime("MAP2LOOP Project init took ", postInit-start)

        currentProgress = 20.0
        currentProgressText = "Map2Loop project config update"

        proj.update_config(
            out_dir=m2lDataDir,
            overwrite="true",
            bbox_3d=bbox,
            proj_crs=proj_crs,
            loopFilename=loopFilename,
            clut_path="source_data\\clutfile.csv",
            run_flags=m2lParams,
            quiet=m2lQuietMode  # Options are 'None', 'no-figures', 'all'
        )
        postConfig = time.time()
        printTime("MAP2LOOP project config took ", postConfig-postInit)

        currentProgress = 35.0
        currentProgressText = "Map2Loop project main run process"

        # proj.run(**m2lParams)
        proj.run()
        postRun = time.time()
        printTime("MAP2LOOP Project run took ", postRun-postConfig)

        currentProgress = 55.0
        currentProgressText = "Post Map2Loop Project File update"

        proj.config.update_projectfile()
        proj.config.export_png()
        postProjectFile = time.time()
        printTime("MAP2LOOP Project File Upload took ", postProjectFile-postRun)

        currentProgress = 95.0
        currentProgressText = "Map2Loop Clean up"

        printTime("Total map2loop time is ", time.time()-start)

        df = pandas.DataFrame(data=bbox,index=[0])
        df = df.rename(columns={"base":"lower","top":"upper"})
        df.to_csv(m2lDataDir+"/tmp/bbox.csv",index=False)

        time.sleep(1)
        currentProgress = 0.0
        currentProgressText = "Map2Loop Completed"

    except Exception as e:
        currentProgress = 666.0
        currentProgressText = "Map2Loop Exception Thrown"
        errors += "PythonError: \n" + traceback.format_exc() + '\n' + repr(e)
        print(errors)
        currentProgressText = "LoopStructural exception Thrown:" + errors

x = threading.Thread(target=threadFunc,
    args=[loopFilename,
          m2lDataDir,
          m2lFiles,
          m2lParams,
          m2lQuietMode,
          bbox,
          proj_crs
         ])

try:
    x.start()
except Exception as e:
    print(e)
