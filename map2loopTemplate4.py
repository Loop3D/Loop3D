from map2loop.project import Project
import LoopProjectFile
# from threading import Timer
import time

# Accept loopFilename from python state otherwise default
if ('loopFilename' not in vars() and 'loopFilename' not in globals()):
    loopFilename = "qldNew2.loop3d"

# TODO: Add utm/gda zone when that can happen
# Collect extents from loopFilename
resp = LoopProjectFile.Get(loopFilename,"extents")
if resp["errorFlag"]: print(resp["errorString"])
else: boundaries = resp["value"]["utm"][2:] + resp["value"]["depth"]

bbox = {"minx":boundaries[0], "maxx":boundaries[1],"miny":boundaries[2],"maxy":boundaries[3],"base":boundaries[5],"top":boundaries[4]}
bbox2=str(bbox["minx"])+","+str(bbox["miny"])+","+str(bbox["maxx"])+","+str(bbox["maxy"])

# print(bbox2)
structure_url='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=outcrops_28355&bbox='+bbox2+'&srsName=EPSG:28355'
geology_url='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_geol_dissolved_join_fix_mii_clip_wgs84&bbox='+bbox2+'&srsName=EPSG:28355'
mindep_url='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_mindeps_28355&bbox='+bbox2+'&srsName=EPSG:28355'
fault_url='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_faults_folds_28355&bbox='+bbox2+'&srsName=EPSG:28355'
fold_url='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_faults_folds_28355&bbox='+bbox2+'&srsName=EPSG:28355'
# strat_graph_file='source_data/graph_strat_NONE.gml'
# Temp override
geology_url='qld_tmp_data/qld_geol_asud.shp'
metadata="qld_tmp_data/qld_meta.hjson"

# geology_url = "data/hams_test.shp"
# fault_url="data/GEOS_GEOLOGY_LINEARSTRUCTURE_500K_GSD.shp"
# fold_url="data/"
# structure_url="data/hams2_structure.shp"
# mindep_url="data/mindeps_2018.shp"

# Override temporarily

start = time.time()
proj = Project(state="QLD",
               remote=True,
               geology_file=geology_url,
               fault_file=fault_url,
               fold_file=fold_url,
               structure_file=structure_url,
               mindep_file=mindep_url,
               metadata=metadata
               )
postInit = time.time()               
print("MAP2LOOP Project init takes " + str(postInit-start) + " seconds")
proj.update_config(
                out_dir="gibberish",
                bbox_3d=bbox,
                step_out=0.1,
                dtm_crs="EPSG:4326",
                proj_crs="EPSG:28355",
                quiet=True
                )
postConfig = time.time()
print("MAP2LOOP Project config takes " + str(postConfig-postInit) + " seconds")
# proj.plot()
proj.run()
postRun = time.time()
print("MAP2LOOP Project run takes " + str(postRun-postConfig) + " seconds")

print("total map2loop time is " + str(time.time()-start))
# Extract data from pandas/geopandas and put into project file
# resp = LoopProjectFile.Set(loopFilename,"faults",faults)
# if resp["errorFlag"]: print(resp["errorString"])

# resp = LoopProjectFile.Set(loopFilename,"faultObservations",faultObservations)
# if resp["errorFlag"]: print(resp["errorString"])

# ...