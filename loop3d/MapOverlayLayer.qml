import QtQuick 2.14
import QtLocation 5.14
import QtPositioning 5.14

MapPolygon {
    layer.enabled: true
    opacity: 1.0
    color: "#55aa0000"
    path: [
        QtPositioning.coordinate(project.minLatitude, project.minLongitude),
        QtPositioning.coordinate(project.maxLatitude, project.minLongitude),
        QtPositioning.coordinate(project.maxLatitude, project.maxLongitude),
        QtPositioning.coordinate(project.minLatitude, project.maxLongitude)
    ]
    border.width: 2
}

