from map2loop.project import Project
import LoopProjectFile
import time
import pandas
import traceback
import os

def findGlobalVariable(varName, defaultValue):
    if (varName not in vars() and varName not in globals()):
        globals()[varName] = defaultValue

# Accept loopFilename and m2lDataDir from python state otherwise default
findGlobalVariable("loopFilename","default.loop3d")
findGlobalVariable("m2lDataDir","m2l_data")
findGlobalVariable("m2lParams",{})
findGlobalVariable("m2lFiles",{})
findGlobalVariable("m2lQuietMode",'all')

if(not os.path.isdir(m2lDataDir)):
    os.mkdir(m2lDataDir)
if(not os.path.isdir(m2lDataDir + "/tmp")):
    os.mkdir(m2lDataDir + "/tmp")
if(not os.path.isdir(m2lDataDir + "/output")):
    os.mkdir(m2lDataDir + "/output")
if(not os.path.isdir(m2lDataDir + "/dtm")):
    os.mkdir(m2lDataDir + "/dtm")
if(not os.path.isdir(m2lDataDir + "/vtk")):
    os.mkdir(m2lDataDir + "/vtk")
if(not os.path.isdir(m2lDataDir + "/graph")):
    os.mkdir(m2lDataDir + "/graph")

# Set default values for boundaries and projection
proj_crs = {"init": "EPSG:28350"}
boundaries = [500102.854, 603064.443, 7455392.3, 7567970.26, -12000.0, 1200.0]

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

errors = ""
try:
    start = time.time()
    proj = Project(**m2lFiles)
    postInit = time.time()               
    print("MAP2LOOP Project init took " + str(postInit-start) + " seconds")

    proj.update_config(
        out_dir=m2lDataDir,
        overwrite="true",
        bbox_3d=bbox,
        proj_crs=proj_crs,
        loopFilename=loopFilename,
        quiet=m2lQuietMode  # Options are 'None', 'no-figures', 'all'
    )
    postConfig = time.time()
    print("MAP2LOOP Project config took " + str(postConfig-postInit) + " seconds")

    print("Min Fault Length is " + str(m2lParams["min_fault_length"]))
    proj.run(**m2lParams)
    postRun = time.time()
    print("MAP2LOOP Project run took " + str(postRun-postConfig) + " seconds")

    proj.config.update_projectfile()
    postProjectFile = time.time()
    print("MAP2LOOP Project File Upload took " + str(postProjectFile-postRun) + " seconds")

    print("Total map2loop time is " + str(time.time()-start))

    df = pandas.DataFrame(data=bbox,index=[0])
    df = df.rename(columns={"base":"lower","top":"upper"})
    df.to_csv(m2lDataDir+"/tmp/bbox.csv",index=False)

except Exception as e:
    errors += "PythonError: \n" + traceback.format_exc() + '\n' + repr(e)
