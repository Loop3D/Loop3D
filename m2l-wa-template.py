from map2loop.project import Project
import LoopProjectFile
import time
import pandas
import traceback
import os

# Accept loopFilename from python state otherwise default
if ("loopFilename" not in vars() and "loopFilename" not in globals()):
    loopFilename = "default.loop3d"
if ("m2l_data_dir" not in vars() and "m2l_data_dir" not in globals()):
    m2l_data_dir = "m2l_data"

if(not os.path.isdir(m2l_data_dir)):
    os.mkdir(m2l_data_dir)
if(not os.path.isdir(m2l_data_dir + "/tmp")):
    os.mkdir(m2l_data_dir + "/tmp")
if(not os.path.isdir(m2l_data_dir + "/output")):
    os.mkdir(m2l_data_dir + "/output")
if(not os.path.isdir(m2l_data_dir + "/dtm")):
    os.mkdir(m2l_data_dir + "/dtm")
if(not os.path.isdir(m2l_data_dir + "/vtk")):
    os.mkdir(m2l_data_dir + "/vtk")
if(not os.path.isdir(m2l_data_dir + "/graph")):
    os.mkdir(m2l_data_dir + "/graph")

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
    proj = Project(
        geology_file=geology_url,
        fault_file=fault_url,
        fold_file=fold_url,
        structure_file=structure_url,
        mindep_file=mindep_url,
        metadata=metadata
    )
    postInit = time.time()               
    print("MAP2LOOP Project init took " + str(postInit-start) + " seconds")

    proj.update_config(
        out_dir=m2l_data_dir,
        overwrite="true",
        bbox_3d=bbox,
        proj_crs=proj_crs,
        loopFilename=loopFilename,
        quiet='all'  # Options are 'None', 'no-figures', 'all'
    )
    postConfig = time.time()
    print("MAP2LOOP Project config took " + str(postConfig-postInit) + " seconds")

    proj.run()
    postRun = time.time()
    print("MAP2LOOP Project run took " + str(postRun-postConfig) + " seconds")

    proj.config.update_projectfile()
    postProjectFile = time.time()
    print("MAP2LOOP Project File Upload took " + str(postProjectFile-postRun) + " seconds")

    print("Total map2loop time is " + str(time.time()-start))

    df = pandas.DataFrame(data=bbox,index=[0])
    df = df.rename(columns={"base":"lower","top":"upper"})
    df.to_csv(m2l_data_dir+"/tmp/bbox.csv",index=False)

except Exception as e:
    errors += "PythonError: \n" + traceback.format_exc() + '\n' + repr(e)
