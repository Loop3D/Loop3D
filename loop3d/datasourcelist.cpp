#include "datasourcelist.h"
#include <fstream>
#include "tokenise.h"
#include <QDebug>
#include <netcdf>

DataSourceList::DataSourceList(QObject *parent) : QObject(parent)
{

}

bool DataSourceList::setDataSourceAt(int index, const DataSourceItem& dataSource)
{
    if (index < 0 || index >= dataSources.size())
        return false;

    dataSources[index] = dataSource;
    return true;

}

void DataSourceList::loadDataSources(QString resourceFile)
{
    resourceFilename = resourceFile;
    std::ifstream dataFile;
    qDebug() << "Trying to open file " << resourceFile;
    dataFile.open(resourceFilename.toStdString(), std::ios::in);
    if (dataFile.is_open()) {
        std::string line;
        while (getline(dataFile,line)) {
            std::vector<std::string> tokens;
            StringTokenise::tokenise(line, tokens," \t,");
            if (tokens.size() >= 3 && tokens[1] == "GROUP") {
                appendGroup(tokens[2].c_str(), tokens[0].c_str());
            } else if (tokens.size() >= 4) {
//                qDebug() << "Loading " << tokens[2].c_str() << " to group " << tokens[0].c_str() << " with url " << tokens[3].c_str() << " in format(" << tokens[1].c_str() << ")";
                appendItem(tokens[2].c_str(), tokens[0].c_str(), tokens[3].c_str(), tokens[1].c_str());
            }
        }
        dataFile.close();
    } else {
        // Load default data sources (TODO: fill in proper URLs for these sources)
        qDebug() << "Failed to open file (" << resourceFile << "), Using default Data Sources";
        appendGroup("Digital Elevation Map", "DEM");
        appendItem("Unidata DEM", "DEM", "");
        appendItem("GeoView WA DEM", "DEM", "");
        appendItem("Local Data DEM", "DEM", "");
        appendGroup("Gravity Variation Map", "GRAV");
        appendItem("nci.org.au Grav","GRAV", "");
        appendItem("GeoView WA Grav", "GRAV", "");
        appendItem("Local Data Grav", "GRAV", "");
        appendGroup("Magnetic Variation Map", "MAG");
        appendItem("nci.org.au Mag", "MAG", "");
        appendItem("GeoView WA Mag", "MAG", "");
        appendGroup("Geological Map", "GEOL");
        appendItem("Unknown Source Geol", "GEOL", "");
        appendItem("GeoView WA Geol", "GEOL", "");
        appendItem("Local Data Geol", "GEOL", "");
    }
//    std::sort(dataSources.begin(),dataSources.end(),order);
}

bool DataSourceList::appendItem(QString name, QString group, QString url, QString format)
{
    DataSourceItem dataSource;
    dataSource.name = name;
    dataSource.group = group;
    dataSource.isParent = false;
    dataSource.isExpanded = false;
    dataSource.dlState = "";
    dataSource.url = url;
    dataSource.format = format;

    preItemAppended();
    dataSources.append(dataSource);
    postItemAppended();
    return 1;
}

bool DataSourceList::appendGroup(QString name, QString group)
{
    DataSourceItem dataSource;
    dataSource.name = name;
    dataSource.group = group;
    dataSource.isParent = true;
    dataSource.isExpanded = false;
    dataSource.dlState = "";
    dataSource.url = "";
    dataSource.format = "";

    preItemAppended();
    dataSources.append(dataSource);
    postItemAppended();
    return 1;
}

bool DataSourceList::removeItem(int index)
{
    if (index < 0 || index >= dataSources.size()) return false;

    preItemRemoved(index);
    dataSources.removeAt(index);
    postItemRemoved();
    return true;
}

QVector<DataSourceItem> DataSourceList::getDataSources() const
{
    return dataSources;
}

void DataSourceList::expandGroup(int index)
{
    if (index < 0 || index >= dataSources.size()) return;
    QString group = dataSources.at(index).group;
    for (auto it=dataSources.begin();it!=dataSources.end();++it) {
        if (!it->isParent && it->group == group) it->isExpanded = !it->isExpanded;
    }
}


/*########## DataSourceItem ###############*/

void DataSourceItem::startDownload()
{
    qDebug() << "Starting downloading of item " << name << " from " << url << " with data format (" << format << ")";

    if (format == "netCDF") {
        try {
            // Create netCDF link to URL
            netCDF::NcFile dataFile(url.toStdString().c_str(),netCDF::NcFile::read);
            qDebug() << "File opened looking for dimension lat";
            netCDF::NcDim ncDimLat = dataFile.getDim("lat");
            qDebug() << "File opened looking for dimension lon";
            netCDF::NcDim ncDimLong = dataFile.getDim("lon");
            qDebug() << "lat/long dimensions are: " << ncDimLat.getSize() << "x" << ncDimLong.getSize();
            float* data = static_cast<float*>(malloc(sizeof(float)*ncDimLat.getSize()*ncDimLong.getSize()));
            if(!data) {
                qDebug() << "Failed to allocate enough memory for band1";
                return;
            }
            netCDF::NcVar ncData = dataFile.getVar("Band1");
            if (ncData.isNull()) {
                qDebug() << "No data called Band1 found in dataset";
                return;
            }
            ncData.getVar(data);
            for (unsigned long long i=0;i<100;i++)
                for (unsigned long long j=0;j<100;j++)
                    qDebug() << data[i+j*ncDimLat.getSize()];

            // establish data to be downloaded extents (lat,long) or northing, easting with stride and final resolution

            // download into a gridded structure

            // save out to current project netCDF file

            // Load into an image for display
        } catch (netCDF::exceptions::NcException& e) {
            qDebug() << e.what();
            qDebug() << "FAILURE***********************";
            return;
        }
    } else {
        qDebug() << "Unknown data format (" << format << ")";
    }
}
