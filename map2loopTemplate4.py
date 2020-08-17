from map2loop.project import Project

# Accept loopFilename from python state otherwise default
if ('loopFilename' not in vars() and 'loopFilename' not in globals()):
    loopFilename = "small.loop3d"

# Accept data file/url names from python state
# Override temporarily
geology_url = "data/hams_test.shp"
fault_url="data/GEOS_GEOLOGY_LINEARSTRUCTURE_500K_GSD.shp"
fold_url=""
structure_url="data/hams2_structure.shp"
mindep_url="data/mindeps_2018.shp"

# TODO: Add utm/gda zone when that can happen
# Collect extents from loopFilename
resp = LoopProjectFile.Get(loopFilename,"extents")
if resp["errorFlag"]: print(resp["errorString"])
else: boundaries = resp["value"]["utm"][2:] + resp["value"]["depth"]
# Override temporarily
bbox = {minx=500057, maxx=603028,miny=7455348,maxy=7567953,base=-8200,top=1200}

proj = Project(geology_file=geology_url,
               fault_file=fault_url,
#               fold_file=fold_url,
               structure_file=structure_url,
               mindep_file=mindep_url)

proj.update_config(bbox_3d=bbox, step_out=0.1)

# proj.plot()
proj.run()

# Extract data from pandas/geopandas and put into project file
# resp = LoopProjectFile.Set(loopFilename,"faults",faults)
# if resp["errorFlag"]: print(resp["errorString"])

# resp = LoopProjectFile.Set(loopFilename,"faultObservations",faultObservations)
# if resp["errorFlag"]: print(resp["errorString"])

# ...