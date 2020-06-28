import 'package:flutter/material.dart';
import 'package:flutter_map/flutter_map.dart';
import 'package:flutter_map/plugin_api.dart';
import 'package:latlong/latlong.dart';
import 'package:location/location.dart';
import 'package:pedantic/pedantic.dart';
import 'package:provider/provider.dart';

import 'package:rocket/logic/connection_manager.dart';
import 'package:rocket/ui/custom_icons.dart';
import 'package:rocket/ui/rocket_3d.dart';

class RocketMap extends StatefulWidget {
  @override
  _RocketMapState createState() => _RocketMapState();
}

class _RocketMapState extends State<RocketMap> {
  LatLng rocketPosition;
  LatLng get userPosition => userLocationData == null
      ? null
      : LatLng(userLocationData.latitude, userLocationData.longitude);
  LocationData userLocationData;
  MapController mapController = MapControllerImpl();

  @override
  void initState() {
    super.initState();

    context.read<ConnectionManager>().gpsPacketStream.listen((packet) {
      if (packet.isValid) {
        setState(() {
          rocketPosition = LatLng(packet.latitude, packet.longitude);
        });
      }
    });

    unawaited(initLocation());
  }

  Future<void> initLocation() async {
    var location = Location();

    if (!await location.serviceEnabled()) {
      if (!await location.requestService()) {
        return;
      }
    }

    while (await location.hasPermission() == PermissionStatus.denied) {
      await location.requestPermission();
    }

    location.onLocationChanged.listen((event) => setState(() {
          userLocationData = event;
        }));
  }

  @override
  Widget build(BuildContext context) {
    return Card(
      child: SizedBox(
        // height: 300,
        child: userPosition == null
            ? const Center(
                child: Text('Please allow location access to use the map'))
            : Stack(
                children: [
                  FlutterMap(
                    mapController: mapController,
                    options: MapOptions(
                        center: LatLng(
                            userPosition.latitude, userPosition.longitude),
                        zoom: 17.0,
                        maxZoom: 18),
                    layers: [
                      TileLayerOptions(
                        urlTemplate:
                            'https://api.mapbox.com/v4/{id}/{z}/{x}/{y}@2x.png?access_token={accessToken}',
                        additionalOptions: {
                          'accessToken':
                              'pk.eyJ1IjoiYXBwamFja3N0dWRpbyIsImEiOiJja2Fsa3ZrOHQwdTNjMnNsYjBwN2tiZzRiIn0.GTf8dtPqrn3GZ7VubiA4dw',
                          'id': 'mapbox.satellite',
                        },
                      ),
                      MarkerLayerOptions(
                        markers: [
                          if (userPosition != null)
                            PositionMarker(userPosition, Colors.blue, null),
                          if (rocketPosition != null)
                            PositionMarker(
                                rocketPosition, Colors.red, CustomIcons.rocket),
                        ],
                      ),
                    ],
                  ),
                  Positioned(
                    right: 10,
                    bottom: 10,
                    child: Column(
                      children: [
                        FloatingActionButton(
                          backgroundColor:
                              rocketPosition == null ? Colors.grey : Colors.red,
                          child: const Icon(CustomIcons.rocket),
                          onPressed: rocketPosition == null
                              ? null
                              : () {
                                  mapController.move(
                                      rocketPosition, mapController.zoom);
                                },
                        ),
                        const Divider(height: 10),
                        FloatingActionButton(
                          backgroundColor: Colors.blue,
                          child: const Icon(Icons.my_location),
                          onPressed: () {
                            mapController.move(
                                userPosition, mapController.zoom);
                          },
                        ),
                      ],
                    ),
                  ),
                  Positioned(
                    right: 0,
                    top: 0,
                    child: Rocket3d(),
                  )
                ],
              ),
      ),
    );
  }
}

class PositionMarker extends Marker {
  PositionMarker(LatLng point, Color color, IconData iconData)
      : super(
          point: point,
          width: 20,
          height: 20,
          builder: (context) => FloatingActionButton(
            onPressed: null,
            backgroundColor: color,
            child: iconData == null ? null : Icon(iconData, size: 10),
          ),
        );
}
