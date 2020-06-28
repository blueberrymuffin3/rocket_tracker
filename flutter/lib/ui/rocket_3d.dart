import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:flutter_cube/flutter_cube.dart';
import 'package:rocket/logic/connection_manager.dart';

class Rocket3d extends StatefulWidget {
  @override
  _Rocket3dState createState() => _Rocket3dState();
}

class QuaternionTween extends Tween<Quaternion> {
  QuaternionTween()
      : super(begin: Quaternion.identity(), end: Quaternion.identity());

  @override
  Quaternion lerp(double t) {
    return begin + (end - begin).scaled(t);
  }
}

class _Rocket3dState extends State<Rocket3d> with TickerProviderStateMixin {
  final _saturnV = Object(fileName: 'assets/rocket/saturn_v.obj');
  Scene _scene;

  AnimationController controller;
  var tween = QuaternionTween();

  @override
  void initState() {
    super.initState();

    context.read<ConnectionManager>().imuPacketStream.listen((packet) {
      tween.begin = tween.evaluate(controller);
      tween.end = Quaternion(packet.x, packet.y, packet.z, packet.w);
      controller.forward(from: 0.0);
    });

    controller = AnimationController(
        duration: const Duration(milliseconds: 500), vsync: this);
    controller.addListener(() {
      _saturnV.transform
          .setRotation(tween.evaluate(controller).asRotationMatrix());
      _scene.update();
    });
    controller.forward();
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      height: 100,
      width: 100,
      color: const Color.fromARGB(200, 0, 0, 0),
      child: ClipRect(
        child: Cube(
          interactive: false,
          onSceneCreated: (Scene scene) {
            _scene = scene;
            scene.world.add(_saturnV);
            scene.camera.position.z = .6;
            scene.camera.position.y = 0;
          },
        ),
      ),
    );
  }
}
