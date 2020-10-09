from map2loop.project import Project
import LoopProjectFile
import time
import pandas

# Accept loopFilename from python state otherwise default
if ('loopFilename' not in vars() and 'loopFilename' not in globals()):
    loopFilename = "default.loop3d"

if ('m2l_data_dir' not in vars() and 'm2l_data_dir' not in globals()):
    m2l_data_dir = "m2l_data"

state = "WA"
proj_crs = {'init': 'EPSG:28350'}

# TODO: Add utm/gda zone when that can happen
# Collect extents from loopFilename
resp = LoopProjectFile.Get(loopFilename, "extents")
if resp["errorFlag"]:
    boundaries = [500102.854, 603064.443,
                  7455392.3, 7567970.26,-1200.0, 12000.0]
    print(resp["errorString"])
else:
    boundaries = resp["value"]["utm"][2:] + resp["value"]["depth"]
    proj_crs = {'init': 'EPSG:'+str(28300+resp["value"]["utm"][0])}

bbox = {"minx": boundaries[0], "maxx": boundaries[1], "miny": boundaries[2],
        "maxy": boundaries[3], "base": boundaries[4], "top": boundaries[5]}

start = time.time()
proj = Project(
    state=state,
    remote=True,
    metadata='https://gist.githubusercontent.com/yohanderose/8f843de0dde531f009a3973cbdadcc9f/raw/918f412ae488ce1a6bca188306f7730061ecf551/meta_remote.hjson'
)
postInit = time.time()               
print("MAP2LOOP Project init took " + str(postInit-start) + " seconds")

proj.update_config(
    out_dir=m2l_data_dir,
    overwrite=True,
    bbox_3d=bbox,
    proj_crs=proj_crs,
    quiet=True
)
postConfig = time.time()
print("MAP2LOOP Project config took " + str(postConfig-postInit) + " seconds")

proj.run()
postRun = time.time()
print("MAP2LOOP Project run took " + str(postRun-postConfig) + " seconds")

proj.config.update_projectfile(loopFilename)
postProjectFile = time.time()
print("MAP2LOOP Project File Upload took " + str(postProjectFile-postRun) + " seconds")

proj.config.export_png()
postPNG = time.time()
print("MAP2LOOP PNG export took " + str(postPNG-postProjectFile) + " seconds")

print("Total map2loop time is " + str(time.time()-start))

df = pandas.DataFrame(data=bbox,index=[0])
df = df.rename(columns={"base":"lower","top":"upper"})
df.to_csv(m2l_data_dir+'/tmp/bbox.csv',index=False)