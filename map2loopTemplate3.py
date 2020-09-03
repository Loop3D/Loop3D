import subprocess
from map2loop import m2l_topology
from map2loop import m2l_utils
from map2loop import m2l_geometry
from map2loop import m2l_interpolation
from map2loop import m2l_export
from map2loop import m2l_map_checker
import LoopProjectFile
import geopandas
import pandas
from shapely.geometry import Polygon
# import urllib
import matplotlib
import rasterio
import numpy
import re
import os
import networkx
import time

if ('loopFilename' not in vars() and 'loopFilename' not in globals()):
    loopFilename = "small.loop3d"
#print(loopFilename)

Australia=True

region='qld'

boundaries = [500057,603028,7455348,7567953,-1200,8200] # UTM S50
if (region=='qld'):
    boundaries = [-334514,-292074,7713303,7739799,-1200,8200]  # UTM S55
    # boundaries = [-333918,-292641,7714302,7738831,-1200,8200]  # UTM S55
elif (region=='nsw'):
    boundaries = [101768,270539,6282407,6383446,1200,8200] # UTM S56


resp = LoopProjectFile.Get(loopFilename,"extents")
if resp["errorFlag"]: print(resp["errorString"])
else: boundaries = resp["value"]["utm"][2:] + resp["value"]["depth"]

minEasting = boundaries[0]
maxEasting = boundaries[1]
minNorthing = boundaries[2]
maxNorthing = boundaries[3]

minx = boundaries[0]
miny = boundaries[2]
maxx = boundaries[1]
maxy = boundaries[3]
bbox2=str(minx)+","+str(miny)+","+str(maxx)+","+str(maxy)

#CRS

src_crs = 'epsg:4326'  # coordinate reference system for imported dtms (geodetic lat/long WGS84)
dst_crs = 'epsg:28355' # coordinate system for data

lat_point_list = [miny, miny, maxy, maxy, maxy]
lon_point_list = [minx, maxx, maxx, minx, minx]
bbox_geom = Polygon(zip(lon_point_list, lat_point_list))
polygon = geopandas.GeoDataFrame(index=[0], crs=dst_crs, geometry=[bbox_geom]) 
bbox=(minx,miny,maxx,maxy)


data_dir="new_tmp_data/"



##### qld config file #####

model_top=1200
model_base=-3200

#PATHS

local_paths=False       #flag to use local or WFS source for data inputs (True = local)


data_path=''
clut_path=''


structure_file='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=outcrops_28355&bbox='+bbox2+'&srsName=EPSG:28355'
geology_file='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_geol_dissolved_join_fix_mii_clip_wgs84&bbox='+bbox2+'&srsName=EPSG:28355'
mindep_file='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_mindeps_28355&bbox='+bbox2+'&srsName=EPSG:28355'
fault_file='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_faults_folds_28355&bbox='+bbox2+'&srsName=EPSG:28355'
fold_file='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_faults_folds_28355&bbox='+bbox2+'&srsName=EPSG:28355'


#CODES AND LABELS 
# these refer to specific fields (codes) in GIS layer or database that contain the info needed for these calcs and text substrings (labels) in the contents of these fields
c_l= {
#Orientations
  "d": "dip_plunge",                  #field that contains dip information
  "dd": "azimuth",             #field that contains dip direction information
  "sf": 'structure',             #field that contains information on type of structure
  "bedding": 'BEDDING',            #text to search for in field defined by sf code to show that this is a bedding measurement
  "otype": 'dip direction',            #flag to determine measurement convention (currently 'strike' or 'dip direction')
  "bo": "facing",             #field that contains type of foliation
  "btype": 'DOWN',            #text to search for in field defined by bo code to show that this is an overturned bedding measurement
#Stratigraphy
  "g": 'Parent_Nam',               #field that contains coarser stratigraphic coding
  "g2": 'Parent_Nam',              #field that contains alternate coarser stratigraphic coding if 'g' is blank
  "c": 'Stratigrap',                 #field that contains finer stratigraphic coding
  "ds": 'ROCK_TYPE',           #field that contains information about lithology
  "u": 'RU_NAME',             #field that contains alternate stratigraphic coding (not used??)
  "r1": 'Intrusive',           #field that contains  extra lithology information
  "r2": 'LITH_SUMM',           #field that contains even more lithology information
  "sill": 'sill',              #text to search for in field defined by ds code to show that this is a sill
  "intrusive": 'Y',    #text to search for in field defined by r1 code to show that this is an intrusion
  "volcanic": 'VOLCANIC',      #text to search for in field defined by ds code to show that this is an volv=canic (not intrusion)
#Mineral Deposits
  "msc": 'site_no',          #field that contains site code of deposit
  "msn": 'occur_name',         #field that contains short name of deposit
  "mst": 'mine_statu',         #field that contains site type of deposit
  "mtc": 'main_commo',         #field that contains target commodity of deposit
  "mscm": 'main_com_1',        #field that contains site commodity of deposit
  "mcom": 'all_commod',        #field that contains commodity group of deposit
  "minf": 'Infrastructure',    #text to search for in field defined by mst code that shows site to ignore
#Timing
  "min": 'Top_Mini_1',         #field that contains minimum age of unit defined by ccode
  "max": 'ASUD_AGE',         #field that contains maximum age of unit defined by ccode
#faults and folds
  "f": 'type',              #field that contains information on type of structure
  "fault": 'Fault',            #text to search for in field defined by f code to show that this is a fault
  "ff": 'type',              #field that contains information on type of structure
  "fold": 'Fold',           #text to search for in field defined by f code to show that this is a fold axial trace
  "fdip": 'dip',               # field for numeric fault dip value
  "fdipnull": '0',         # text to search for in field defined by fdip to show that this has no known dip
  "fdipdir": 'dip_dir',        # field for text fault dip direction value 
  "fdipdir_flag": 'alpha',        # flag for text fault dip direction type num e.g. 045 or alpha e.g. southeast    
  "fdipest": 'dip_est',        # field for text fault dip estimate value
  "fdipest_vals": 'gentle,moderate,steep',        # text to search for in field defined by fdipest to give fault dip estimate in increasing steepness
  "n": 'name',                 #field that contains information on name of fault (not used??)
  "t": 'id_desc',                 #field that contains information on type of fold
  "syn": 'Syncline',           #text to search for in field defined by t to show that this is a syncline
#ids
  "o": 'objectid',             #field that contains unique id of geometry object
  "gi": 'objectid'            #field that contains unique id of structure point
}

#DECIMATION

orientation_decimate=0  #store every nth orientation (in object order) 0 = save all
contact_decimate=10     #store every nth contact point (in object order) 0 = save all
fault_decimate=5        #store every nth fault point (in object order) 0 = save all
fold_decimate=5         #store every nth fold axial trace point (in object order) 0 = save all

#INTERPOLATION

gridx=50                #x grid dimensions (no of points, not distance) for interpolations
gridy=50                #x grid dimensions (no of points, not distance) for interpolations
scheme='scipy_rbf'      #interpolation scheme
dist_buffer=5           #buffer distance for clipping points by faults (in metres or same units as dst_crs)
intrusion_mode=0        # 1 all intrusions exluded from basal contacts, 0 only sills
use_interpolations=False    # flag to use interpolated orientations or not.

#ASSUMPTIONS

pluton_dip=45           #surface dip of pluton contacts
pluton_form='domes'     #saucers: \__+_+__/  batholith: +/       \+   domes: /  +  + \  pendant: +\_____/+
fault_dip=90            #surface dip of faults

#DERIVED AND FIXED PATHS

m2m_cpp_path='../'

graph_path=data_dir+'graph/'
tmp_path=data_dir+'tmp/'
#data_path=data_dir+'data/'
dtm_path=data_dir+'dtm/'
output_path=data_dir+'output/'
vtk_path=data_dir+'vtk/'

fault_file_csv=tmp_path+'fault_file.csv'
structure_file_csv=tmp_path+'structure_file.csv'
geology_file_csv=tmp_path+'geology_file_file.csv'
mindep_file_csv=tmp_path+'mindep_file.csv'

# fault_file=data_path+fault_file
# fold_file=data_path+fold_file
# structure_file=data_path+structure_file
# geology_file=data_path+geology_file
# mindep_file=data_path+mindep_file

strat_graph_file='source_data/graph_strat_NONE.gml'

dtm_file=data_dir+'dtm.tif'
dtm_reproj_file=data_dir+'dtm_rp.tif'

##### End config File #####
# opportunity to second guess config file or add extra parameters not yet in config file...

fold_decimate=2         
fault_decimate=0
contact_decimate=5
orientation_decimate=0
use_interpolations=True       #use interpolated dips/contacts as additional constraints
use_fat=True                   #use fold axial trace orientation hints
pluton_form='domes'
fault_dip=90
min_fault_length=5000
compute_etc=False
#spacing > 0 gives absolute spacing, spacing <0 gives # of grid points in x direction
spacing=-200
#spacing=500   #grid spacing in metres of interpolation points

workflow={'model_engine':'loopStructural',
    'seismic_section':False,
    'cover_map':False,
    'near_fault_interpolations':False,
    'fold_axial_traces':True,
    'stereonets':True,
    'formation_thickness':True,
    'polarity':False,
    'strat_offset':True,
    'contact_dips':True}

dtb=0
dtb_null=0

# geology_file='../qld_src_data/data/qld_geol_asud.shp'
geology_file='qld_tmp_data/qld_geol_asud.shp'

structure_file,geology_file,fault_file,mindep_file,fold_file=m2l_map_checker.check_map(structure_file,geology_file,fault_file,mindep_file,fold_file,tmp_path,bbox,c_l,dst_crs,local_paths)

geology = geopandas.read_file(geology_file,bbox=bbox)

geology[c_l['g']].fillna(geology[c_l['g2']], inplace=True)
geology[c_l['g']].fillna(geology[c_l['c']], inplace=True)

orientations = geopandas.read_file(structure_file,bbox=bbox)

if(len(orientations)<2):
    raise NameError('Not enough orientations to complete calculations (need at least 2)')

# This crashes Qt at the moment (can't find windows plugin???)
group_girdle=m2l_utils.plot_bedding_stereonets(orientations,geology,c_l,showPlot=False)

misorientation=30
super_groups,use_gcode3=m2l_topology.super_groups_and_groups(group_girdle,tmp_path,misorientation)

print("super_groups=",super_groups)
print("use_gcode3=",use_gcode3)


inputs=('invented_orientations','fat_orientations','contact_orientations')

geology_ll = geopandas.read_file(geology_file,bbox=bbox)

geology_ll[c_l['g']].fillna(geology_ll[c_l['g2']], inplace=True)
geology_ll[c_l['g']].fillna(geology_ll[c_l['c']], inplace=True)
# display(geology_ll.head())
base=geology_ll.plot(column=c_l['c'],figsize=(10,10),edgecolor='#000000',linewidth=0.2)
polygon.plot(ax=base, color='none',edgecolor='black')

hint_flag=False # use GSWA strat database to provide relative age hints
sub_geol = geology_ll[['geometry', c_l['o'],c_l['c'],c_l['g'],c_l['u'],c_l['min'],c_l['max'],c_l['ds'],c_l['r1'],c_l['r2']]]
m2l_topology.save_geol_wkt(sub_geol,geology_file_csv, c_l,hint_flag)

mindep = geopandas.read_file(mindep_file,bbox=bbox)

sub_mindep = mindep[['geometry', c_l['msc'],c_l['msn'],c_l['mst'],c_l['mtc'],c_l['mscm'],c_l['mcom']]]
m2l_topology.save_mindep_wkt(sub_mindep,mindep_file_csv, c_l)

base=sub_mindep.plot()
polygon.plot(ax=base, color='none',edgecolor='black')


orientations = geopandas.read_file(structure_file,bbox=bbox)

sub_pts = orientations[['geometry', c_l['gi'],c_l['d'],c_l['dd']]]

m2l_topology.save_structure_wkt(sub_pts,structure_file_csv,c_l)

base=sub_pts.plot()
polygon.plot(ax=base, color='none',edgecolor='black')

if(os.path.exists(fault_file)):
    lines_ll=geopandas.read_file(fault_file,bbox=bbox)

    sub_lines = lines_ll[['geometry', c_l['o'],c_l['f']]]
    base=sub_lines.plot()
    polygon.plot(ax=base, color='none',edgecolor='black')
else:
    sub_lines=[]


m2l_topology.save_faults_wkt(sub_lines,fault_file_csv,c_l)
# m2l_topology.save_Parfile(m2m_cpp_path,c_l,graph_path,geology_file_csv,fault_file_csv,structure_file_csv,mindep_file_csv,minx,maxx,miny,maxy,500.0,'Fe,Cu,Au,NONE',1)
m2l_topology.save_Parfile("./",c_l,graph_path,geology_file_csv,fault_file_csv,structure_file_csv,mindep_file_csv,minx,maxx,miny,maxy,500.0,'Fe,Cu,Au,NONE',1)
# About to run map2model-cpp code
# m2l_topology.save_Parfile("./",c_l,
    # data_dir+"/graph",
    # data_dir+"/geology.txt",
    # data_dir+"/fault.txt",
    # data_dir+"/structure.txt",
    # data_dir+"/mindep.txt",
    # minEasting,maxEasting,minNorthing,maxNorthing,
    # 500.0,'Fe,Cu,Au,NONE',1)
# This subprocess creates a lot of files in <data_dir>/graph directory
subprocess.run(["C:\workspace\map2loop\m2m_cpp\map2model.exe","Parfile"])
# import subprocess
# import platform
# os.chdir(m2m_cpp_path)
# print(os.getcwd())
# #%system map2model.exe Parfile
# if(platform.system()=='Windows'):
#     subprocess.run(["map2model.exe", "Parfile"])
# else:
#     subprocess.run(["./map2model", "Parfile"])



if(Australia):
    asud_strat_file='source_data/ASUD.csv'
    m2l_topology.use_asud(strat_graph_file, asud_strat_file,graph_path)
    strat_graph_file=graph_path+'ASUD_strat.gml'


G=networkx.read_gml(strat_graph_file,label='id')
selected_nodes = [n for n,v in G.nodes(data=True) if n >=0]
networkx.draw_networkx(G, pos=networkx.kamada_kawai_layout(G), arrows=True, nodelist=selected_nodes)

nlist=list(G.nodes.data('LabelGraphics'))
nlist.sort()
for no in nlist:
    if(no[0]>=0):
        elem=str(no[1]).replace("{'text':","").replace(", 'fontSize': 14}","")
        #second=elem.split(":").replace("'","")
        print(no[0]," ",elem)

# os.chdir('../map2loop')

print(os.getcwd())

#from IPython.core.display import display, HTML
#display(HTML("<style>.container { width:80% !important; }</style>"))

# sys.path.insert(0,"../..")
print(os.getcwd())


print(os.getcwd())

bbox2=str(minx)+","+str(miny)+","+str(maxx)+","+str(maxy)
lat_point_list = [miny, miny, maxy, maxy, maxy]
lon_point_list = [minx, maxx, maxx, minx, minx]
bbox_geom = Polygon(zip(lon_point_list, lat_point_list))
polygon = geopandas.GeoDataFrame(index=[0], crs=dst_crs, geometry=[bbox_geom]) 
bbox=(minx,miny,maxx,maxy)

step_out=0.045 #add (in degrees) so edge pixel from dtm reprojection are not found


polygon_ll=polygon.to_crs(src_crs)

minlong=polygon_ll.total_bounds[0]-step_out
maxlong=polygon_ll.total_bounds[2]+step_out
minlat=polygon_ll.total_bounds[1]-step_out
maxlat=polygon_ll.total_bounds[3]+step_out

print(minlong,maxlong,minlat,maxlat)
downloaded = False
i=0
print('Attempt: 0 ',end='')
while downloaded == False:
    try:
        m2l_utils.get_dtm(dtm_file, minlong,maxlong,minlat,maxlat)
        downloaded=True
    except:
        time.sleep(10)
        i=i+1
        print(' ',i,end='')
    if(i==5):
        raise NameError('map2loop error: Could not access DTM server after 100 attempts')
print('Finished dtm download')

geom_rp=m2l_utils.reproject_dtm(dtm_file,dtm_reproj_file,src_crs,dst_crs)

dtm = rasterio.open(dtm_reproj_file)
# pyplot.imshow(dtm.read(1), cmap='terrain',vmin=0,vmax=1000)
# pyplot.show()



groups,glabels,G = m2l_topology.get_series(strat_graph_file,'id')
# m2l_topology.save_units(G,tmp_path,glabels,Australia,asud_strat_file,showPlot=False)
m2l_topology.save_units(G,tmp_path,glabels,Australia,asud_strat_file)
# Extract point data from structure & geology layers for modelling
##First we readin the structure and map from shapefiles, or wherever...

bbox=(minx,miny,maxx,maxy)
geology = geopandas.read_file(geology_file,bbox=bbox)
geology[c_l['g']].fillna(geology[c_l['g2']], inplace=True)
geology[c_l['g']].fillna(geology[c_l['c']], inplace=True)
geology.plot(cmap="gist_earth")
matplotlib.pyplot.axis('off')
matplotlib.pyplot.margins(0,0)
matplotlib.pyplot.savefig(loopFilename+'.jpg',dpi=1000,quality=100,bbox_inches='tight',pad_inches=0)
# matplotlib.pyplot.show()

structure = geopandas.read_file(structure_file,bbox=bbox)
structure.crs=dst_crs
print(fault_file)
if(os.path.exists(fault_file)):
    faults_clip = geopandas.read_file(fault_file,bbox=bbox)
    faults_clip.crs=dst_crs

sub_pts = structure[['geometry',c_l['d'],c_l['dd'],c_l['sf'],c_l['bo']]] 

base=geology.plot(column=c_l['c'],figsize=(10,10),edgecolor='#000000',linewidth=0.2)
sub_pts.plot(ax=base,edgecolor='black')
faults_clip.plot(ax=base, column=c_l['f'],edgecolor='black')

geol_clip = m2l_utils.explode(geology)
geol_clip.crs = dst_crs
pandas.set_option('display.max_columns', None)
pandas.set_option('display.max_rows',None)
structure_code = geopandas.sjoin(sub_pts, geol_clip, how="left", op="within")

y_point_list = [miny, miny, maxy, maxy, miny]
x_point_list = [minx, maxx, maxx, minx, minx]

bbox_geom = Polygon(zip(x_point_list, y_point_list))

polygo = geopandas.GeoDataFrame(index=[0], crs=dst_crs, geometry=[bbox_geom]) 
is_bed=structure_code[c_l['sf']].str.contains(c_l['bedding'], regex=False) 
    
structure_clip = structure_code[is_bed]
structure_clip.crs = dst_crs


if(c_l['dd']=='strike'):
    structure_clip['azimuth2'] = structure_clip.apply(lambda row: row[c_l['dd']]+90.0, axis = 1)
    c_l['dd']='azimuth2'
    

structure_clip.to_file(tmp_path+'structure_clip.shp')


m2l_topology.save_group(G,tmp_path,glabels,geol_clip,c_l)

m2l_geometry.save_orientations(structure_clip,output_path,c_l,orientation_decimate,dtm,dtb,dtb_null,workflow['cover_map'])

m2l_utils.plot_points(output_path+'orientations.csv',geol_clip, 'formation','X','Y',False,'alpha')
m2l_geometry.create_orientations( tmp_path, output_path, dtm,dtb,dtb_null,workflow['cover_map'],geol_clip,structure_clip,c_l)
ls_dict,ls_dict_decimate=m2l_geometry.save_basal_contacts(tmp_path,dtm,dtb,dtb_null,workflow['cover_map'],geol_clip,contact_decimate,c_l,intrusion_mode)
m2l_geometry.save_basal_no_faults(tmp_path+'basal_contacts.shp',tmp_path+'faults_clip.shp',ls_dict,10,c_l,dst_crs)
contacts=geopandas.read_file(tmp_path+'basal_contacts.shp')

m2l_geometry.save_basal_contacts_csv(contacts,output_path,dtm,dtb,dtb_null,workflow['cover_map'],contact_decimate,c_l)

m2l_utils.plot_points(output_path+'contacts4.csv',geol_clip, 'formation','X','Y',False,'alpha')

basal_contacts=tmp_path+'basal_contacts.shp'
contacts=geopandas.read_file(basal_contacts,bbox=bbox)

orientation_interp,contact_interp,combo_interp=m2l_interpolation.interpolation_grids(geology_file,structure_file,basal_contacts,bbox,spacing,dst_crs,scheme,super_groups,c_l)


f=open(tmp_path+'interpolated_orientations.csv','w')
f.write('X,Y,l,m,n,dip,dip_dir\n')
for row in orientation_interp:
    ostr='{},{},{},{},{},{},{}\n'.format(row[0],row[1],row[2],row[3],row[4],row[5],row[6])
    f.write(ostr)
f.close()
f=open(tmp_path+'interpolated_contacts.csv','w')
f.write('X,Y,l,m,angle\n')
for row in contact_interp:
    ostr='{},{},{},{},{}\n'.format(row[0],row[1],row[2],row[3],row[4])
    f.write(ostr)
f.close()
f=open(tmp_path+'interpolated_combined.csv','w')
f.write('X,Y,l,m,n,dip,dip_dir\n')
for row in combo_interp:
    ostr='{},{},{},{},{},{},{}\n'.format(row[0],row[1],row[2],row[3],row[4],row[5],row[6])
    f.write(ostr)
f.close()

if(spacing<0):
    spacing=-(bbox[2]-bbox[0])/spacing
x=int((bbox[2]-bbox[0])/spacing)+1
y=int((bbox[3]-bbox[1])/spacing)+1
print(x,y)
dip_grid=numpy.ones((y,x))
dip_grid=dip_grid*-999
dip_dir_grid=numpy.ones((y,x))
dip_dir_grid=dip_dir_grid*-999
contact_grid=numpy.ones((y,x))
contact_grid=dip_dir_grid*-999
for row in combo_interp:
    r=int((row[1]-bbox[1])/spacing)
    c=int((row[0]-bbox[0])/spacing)
    dip_grid[r,c]=float(row[5])
    dip_dir_grid[r,c]=float(row[6])

for row in contact_interp:
    r=int((row[1]-bbox[1])/spacing)
    c=int((row[0]-bbox[0])/spacing)
    contact_grid[r,c]=float(row[4])

print('interpolated dips')
# plt.imshow(dip_grid, cmap="hsv",origin='lower',vmin=-90,vmax=90)  
# plt.show()

print('interpolated dip directions')
       
# plt.imshow(dip_dir_grid, cmap="hsv",origin='lower',vmin=0,vmax=360)    
# plt.show()

print('interpolated contacts')
       
# plt.imshow(contact_grid, cmap="hsv",origin='lower',vmin=-360,vmax=360)    
# plt.show()

m2l_geometry.save_faults(tmp_path+'faults_clip.shp',output_path,dtm,dtb,dtb_null,workflow['cover_map'],c_l,fault_decimate,min_fault_length,fault_dip)


import warnings
warnings.filterwarnings('ignore')

if(os.path.exists(fault_file)):
    faults=pandas.read_csv(output_path+'faults.csv')
    faults_len=len(faults)

    if(faults_len>0):
        m2l_interpolation.process_fault_throw_and_near_faults_from_grid(tmp_path,output_path,dtm_reproj_file,dtb,dtb_null,workflow['cover_map'],c_l,dst_crs,bbox,
                                                                scheme,dip_grid,dip_dir_grid,x,y,spacing)


bbox=(minx,miny,maxx,maxy)

pluton_dip=str(pluton_dip)

dist_buffer=10

m2l_geometry.process_plutons(tmp_path,output_path,geol_clip,local_paths,dtm,dtb,dtb_null,workflow['cover_map'],pluton_form,pluton_dip,contact_decimate,c_l)

if(workflow['contact_dips']):
    orientations=pandas.read_csv(output_path+'orientations.csv',",")
    contact_dip=-999
    contact_orientation_decimate=5
    m2l_geometry.save_basal_contacts_orientations_csv(contacts,orientations,geol_clip,tmp_path,output_path,dtm,dtb,
                            dtb_null,workflow['cover_map'],contact_orientation_decimate,c_l,contact_dip,dip_grid,spacing,bbox)

if(workflow['formation_thickness']):

    geology_file=tmp_path+'basal_contacts.shp'
    contact_decimate=5
    null_scheme='null'
    m2l_interpolation.save_contact_vectors(geology_file,tmp_path,dtm,dtb,dtb_null,workflow['cover_map'],bbox,c_l,null_scheme,contact_decimate)
    
    buffer =5000
    max_thickness_allowed=10000

    m2l_geometry.calc_thickness_with_grid(tmp_path,output_path,buffer,max_thickness_allowed,
                                          c_l,bbox,dip_grid,dip_dir_grid,x,y,spacing)

    m2l_geometry.calc_min_thickness_with_grid(tmp_path,output_path,buffer,max_thickness_allowed,
                                          c_l,bbox,dip_grid,dip_dir_grid,x,y,spacing)

    m2l_geometry.normalise_thickness(output_path)
    
    m2l_utils.plot_points(output_path+'formation_thicknesses_norm.csv',geol_clip,'norm_th','x','y',True,'numeric')

if(workflow['fold_axial_traces']   and  os.path.exists(fold_file)):
    folds_clip = geopandas.read_file(fold_file)
    if(len(folds_clip)>0):
        
        m2l_geometry.save_fold_axial_traces(fold_file,output_path,dtm,dtb,dtb_null,workflow['cover_map'],c_l,fold_decimate)

        #Save fold axial trace near-hinge orientations
        fat_step=750         # how much to step out normal to fold axial trace
        close_dip=-999       #dip to assign to all new orientations (-999= use local interpolated dip)

        m2l_geometry.save_fold_axial_traces_orientations(fold_file,output_path,tmp_path,dtm,dtb,dtb_null,workflow['cover_map'],c_l,dst_crs,
                                                         fold_decimate,fat_step,close_dip,scheme,bbox,spacing,dip_grid,dip_dir_grid)


m2l_geometry.tidy_data(output_path,tmp_path,clut_path,use_gcode3,use_interpolations,use_fat,pluton_form,inputs,workflow,c_l)
dtm.close()

if(workflow['polarity']):  # Currently False
    m2l_geometry.save_orientations_with_polarity(output_path+'orientations.csv',output_path,c_l,tmp_path+'basal_contacts.shp',tmp_path+'all_sorts.csv',)
    m2l_utils.plot_points(output_path+'orientations_polarity.csv',geol_clip,'polarity','X','Y',True,'alpha')


if(workflow['strat_offset'] and os.path.exists(fault_file)):
    fault_test=pandas.read_csv(output_path+'fault_dimensions.csv',',')
    if(len(fault_test)>0):
        m2l_geometry.fault_strat_offset(output_path,c_l,dst_crs,output_path+'formation_summary_thicknesses.csv', tmp_path+'all_sorts.csv',tmp_path+'faults_clip.shp',tmp_path+'geol_clip.shp',output_path+'fault_dimensions.csv')
        m2l_utils.plot_points(output_path+'fault_strat_offset3.csv',geol_clip,'min_offset','X','Y',True,'numeric')
        m2l_utils.plot_points(output_path+'fault_strat_offset3.csv',geol_clip,'strat_offset','X','Y',True,'numeric')

if(os.path.exists(fault_file)):
    m2l_topology.parse_fault_relationships(graph_path,tmp_path,output_path)

# if(workflow['model_engine']=='loopStructural'):
#     model_top=1200
#     model_base=-8200

#     import random
#     from datetime import datetime
#     from LoopStructural import GeologicalModel
#     import lavavu
#     from LoopStructural.visualisation import LavaVuModelViewer
#     from LoopStructural import GeologicalModel
#     import logging
#     logging.getLogger().setLevel(logging.ERROR)

#     nowtime=datetime.now().isoformat(timespec='minutes')   
#     model_name='leaflet'+'_'+nowtime.replace(":","-").replace("T","-")
#     os.mkdir(vtk_path+model_name)
#     filename=vtk_path+model_name+'/'+'surface_name_{}.vtk'


#     f=open(tmp_path+'bbox.csv','w')
#     f.write('minx,miny,maxx,maxy,lower,upper\n')
#     ostr='{},{},{},{},{},{}\n'.format(minx,miny,maxx,maxy,model_base,model_top)
#     f.write(ostr)
#     f.close()
#     t1 = time.time()
#     fault_params = {'interpolatortype':'FDI',
#                     'nelements':3e4,
#                     'data_region':.1,
#                     'solver':'pyamg',
#     #                 overprints:overprints,
#                     'cpw':10,
#                     'npw':10}
#     foliation_params = {'interpolatortype':'PLI' , # 'interpolatortype':'PLI',
#                         'nelements':1e5,  # how many tetras/voxels
#                         'buffer':0.8,  # how much to extend nterpolation around box
#                         'solver':'pyamg',
#                         'damp':True}

#     if(not os.path.exists(fault_file)):
#         f=open(output_path + '/fault_displacements3.csv','w')
#         f.write('X,Y,fname,apparent_displacement,vertical_displacement,downthrow_dir\n')
#         f.close()
#         f=open(output_path + '/fault_orientations.csv','w')
#         f.write('X,Y,Z,DipDirection,dip,DipPolarity,formation\n')
#         f.close()
#         f=open(output_path + '/faults.csv','w')
#         f.write('X,Y,Z,formation\n')
#         f.close()
#         f=open(output_path + '/fault-fault-relationships.csv','w')
#         f.write('fault_id\n')
#         f.close()
#         f=open(output_path + '/group-fault-relationships.csv','w')
#         f.write('group\n')
#         f.close()

#         model, m2l_data = GeologicalModel.from_map2loop_directory(data_dir,
#                                                               skip_faults=True,
#                                                               fault_params=fault_params,
#                                                               foliation_params=foliation_params)
#     else:
#         model, m2l_data = GeologicalModel.from_map2loop_directory(data_dir,
#                                                               skip_faults=False,
#                                                               fault_params=fault_params,
#                                                               foliation_params=foliation_params)

#     view = LavaVuModelViewer(model,vertical_exaggeration=1) 
#     view.nsteps = numpy.array([200,200,200])
#     #view.set_zscale(2)
#     view.add_model(cmap='tab20')
#     view.nsteps=numpy.array([50,50,50])
#     view.add_model_surfaces()
#     #view.add_model_surfaces(filename=filename)
#     for sg in model.feature_name_index:
#         if( 'super' in sg):
#             view.add_data(model.features[model.feature_name_index[sg]])
#     view.interactive()  
    
#     for sg in model.feature_name_index:
#         if( 'super' in sg):
#             view.add_data(model.features[model.feature_name_index[sg]])

#     # t2 = time.time()
#     # print("m2l",(t1-t0)/60.0,"LoopStructural",(t2-t1)/60.0,"Total",(t2-t0)/60.0,"minutes")

# if(workflow['model_engine']=='loopStructural'):  
#     dtm = rasterio.open(dtm_reproj_file)
#     if(os.path.exists(fault_file)):
#         f_clip=faults_clip[faults_clip[c_l['f']].str.contains(c_l['fault'])]
#         m2l_export.display_LS_map(model,dtm,geol_clip,f_clip,dst_crs,use_topo=True,use_faults=True)
#     else:
#         f_clip=[]
#         m2l_export.display_LS_map(model,dtm,geol_clip,f_clip,dst_crs,use_topo=True,use_faults=False)

#     dtm.close()

# if(workflow['model_engine']=='loopStructural'):  
#     voxel_size=500
#     sizex=int((maxx-minx)/voxel_size)
#     sizey=int((maxy-miny)/voxel_size)
#     sizez=int((model_top-model_base)/voxel_size)
#     print('voxel_size=',voxel_size,', saved in Z,Y,X order, X=',sizex,', Y=',sizey,', Z=',sizez)
#     print('lower south west corner: west=',minx,', south=',miny,', lower=',model_base)
#     voxels=model.evaluate_model(model.regular_grid(nsteps=(sizey,sizex,sizez),shuffle=False))
#     numpy.savetxt(tmp_path+'voxels.raw', voxels, fmt='%d',delimiter='\t',newline='')
#     print('voxels saved as',tmp_path+'voxels.raw')


################ Sending outputs to loop Project File #########################

# used to give a mean width of each formation
stratigraphicLayers = pandas.read_csv(data_dir+"/output/formation_thicknesses.csv")
thickness = {}
uniqueLayers = stratigraphicLayers['formation'].unique()
for f in uniqueLayers:
    thickness[f] = numpy.mean(stratigraphicLayers[stratigraphicLayers['formation']==f]['thickness'])
stratigraphicLogData = numpy.zeros(uniqueLayers.shape[0],LoopProjectFile.stratigraphicLayerType)
stratigraphicLogData['layerId'] = range(uniqueLayers.shape[0])
stratigraphicLogData['layerId'] += 1
stratigraphicLogData['minAge'] = range(uniqueLayers.shape[0])
stratigraphicLogData['maxAge'] = range(uniqueLayers.shape[0])
stratigraphicLogData['maxAge'] += 0.5
stratigraphicLogData['name'] = uniqueLayers
stratigraphicLogData['enabled'] = 1
stratigraphicLogData['rank'] = 0
stratigraphicLogData['thickness'] = list(thickness.values())
resp = LoopProjectFile.Set(loopFilename,"stratigraphicLog",data=stratigraphicLogData,verbose=True)
if resp["errorFlag"]: print(resp["errorString"])

faults = pandas.read_csv(data_dir+"/output/fault_orientations.csv")
faultEvents = numpy.zeros(faults.shape[0],LoopProjectFile.faultEventType)
faultEvents['name'] = faults['formation']  # The fault eventId is called formation for some reason
faultEvents['enabled'] = 0
faultEvents['rank'] = 0
faultEvents['minAge'] = numpy.arange(1.0,7.0, 6.0/faults.shape[0])
faultEvents['maxAge'] = faultEvents['minAge']
faultEvents['avgDisplacement'] = 0
for i in range(faultEvents.size):
    faults.loc[i,'formation'] = re.sub('.*_','',faults['formation'][i])
tmp = faults['formation']
faultEvents['eventId'] = tmp 

# tmp = faults_clip[~faults_clip['fname'].isnull()]
# lookup = dict(zip(numpy.array(tmp['objectid']),numpy.array(tmp['fname'])))

# def replaceName(row,lookup):
#     key = int(row['eventId'])
#     if key in lookup:
#         val = lookup[key]
#         row['name'] = val
#         row['enabled'] = 1
#     else:
#         pass

# for row in faultEvents:
#     replaceName(row,lookup)

resp = LoopProjectFile.Set(loopFilename,"faultLog",data=faultEvents,verbose=False)
if resp["errorFlag"]: print(resp["errorString"])

faultsData = numpy.zeros(faults.shape[0],LoopProjectFile.faultObservationType)
faultsData['eventId'] = faults['formation']
faultsData['easting'] = faults['X']
faultsData['northing'] = faults['Y']
faultsData['altitude'] = faults['Z']
faultsData['dipDir'] = faults['DipDirection']
faultsData['dip'] = faults['dip']
faultsData['dipPolarity'] = faults['DipPolarity']
resp = LoopProjectFile.Set(loopFilename,"faultObservations",data=faultsData,verbose=True)
if resp["errorFlag"]: print(resp["errorString"])

# each contact contains a location and which formation it is on
contacts = pandas.read_csv(data_dir+"/output/contacts4.csv")
layerIds = []
for form in contacts['formation']:
    a = bytes(form,'ascii')
    if a in stratigraphicLogData['formation']:
        layerIds.append(int(stratigraphicLogData[stratigraphicLogData['formation']==a]['layerId']))
    else:
        layerIds.append(0)
contactsData = numpy.zeros(contacts.shape[0],LoopProjectFile.contactObservationType)
contactsData['layerId'] = layerIds
contactsData['easting'] = contacts['X']
contactsData['northing'] = contacts['Y']
contactsData['altitude'] = contacts['Z']
resp = LoopProjectFile.Set(loopFilename,"contacts",data=contactsData,verbose=True)
if resp["errorFlag"]: print(resp["errorString"])

observations = pandas.read_csv(data_dir+"/output/orientations.csv")
layerIds = []
for form in observations['formation']:
    a = bytes(form,'ascii')
    if a in stratigraphicLogData['formation']:
        layerIds.append(int(stratigraphicLogData[stratigraphicLogData['formation']==a]['layerId']))
    else:
        layerIds.append(0)
observations['layer'] = "s0"
observationsData = numpy.zeros(observations.shape[0],LoopProjectFile.stratigraphicObservationType)
observationsData['layerId'] = layerIds
observationsData['easting'] = observations['X']
observationsData['northing'] = observations['Y']
observationsData['altitude'] = observations['Z']
observationsData['dipDir'] = observations['azimuth']
observationsData['dip'] = observations['dip']
observationsData['dipPolarity'] = observations['polarity']
observationsData['layer'] = observations['layer']
resp = LoopProjectFile.Set(loopFilename,"stratigraphicObservations",data=observationsData,verbose=True)
if resp["errorFlag"]: print(resp["errorString"])



import winsound
duration = 700  # milliseconds
freq = 1100  # Hz
winsound.Beep(freq, duration)
winsound.Beep(freq, duration)
winsound.Beep(freq, duration)
