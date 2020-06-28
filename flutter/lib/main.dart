import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:provider/provider.dart';
import 'package:rocket/logic/connection_manager.dart';
import 'package:rocket/ui/message_log.dart';
import 'package:rocket/ui/rocket_3d.dart';
import 'package:rocket/ui/rocket_map.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await SystemChrome.setPreferredOrientations([DeviceOrientation.portraitUp]);
  runApp(ChangeNotifierProvider(
      create: (_) => ConnectionManager(), lazy: false, child: App()));
}

class App extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData.dark(),
      home: HomePage(),
    );
  }
}

class HomePage extends StatefulWidget {
  @override
  _HomePageState createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  @override
  void initState() {
    super.initState();
    Future(() => context.read<ConnectionManager>().promptConnect(context));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Rocket Tracker'),
        actions: [
          IconButton(
              icon: const Icon(Icons.cancel),
              onPressed: () {
                context.read<ConnectionManager>().promptConnect(context);
              })
        ],
      ),
      body: Column(
        children: [
          MessageLog(),
          Expanded(
            child: RocketMap(),
          ),
        ],
      ),
    );
  }
}
