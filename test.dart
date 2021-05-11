import 'dart:ffi';

import './sort_dart.dart';
import 'dart:async';
import 'dart:io';

dynamic processData() async {
  // var fileContent = await File('./testData.txt').readAsString();
  var fileContent = await File('./data/ADL-Rundle-6/det.txt').readAsString();

  var bbox = [];
  var bbox_per_frame = [];
  // Label index starts from 1
  var current_frame_index = 1;
  var lines = fileContent.toString().split('\n');
  for (int i = 0; i < lines.length - 1; i++) {
    var label = lines[i].split(',');
    if (int.parse(label[0]) != current_frame_index) {
      current_frame_index = int.parse(label[0]);
      bbox.add(bbox_per_frame);
      bbox_per_frame = [];
    }
    var object = [
      double.parse(label[2]),
      double.parse(label[3]),
      double.parse(label[4]),
      double.parse(label[5]),
      double.parse(label[6])
    ];
    bbox_per_frame.add(object);
  }
  bbox.add(bbox_per_frame);
  return bbox;
}

main() async {
  var all_detections = await processData();
  var total_frames = all_detections.length;
  SortDart sortDart = new SortDart(3, 1, 1, 0.3, 0.6, 1);
  var frame_index = 0;
  var startTime = DateTime.now().millisecondsSinceEpoch;
  for (int i = 0; i < total_frames; i++) {
    var detections = all_detections[i];
    var bboxes = [];
    var scores = [];
    var landmarks = [];
    print("****************** FRAME $frame_index ************************");
    detections.forEach((detection) {
      var bbox = [detection[0], detection[1], detection[2], detection[3]];
      var score = detection[4];
      var landmark = [1.0, 1.0, 1.0];
      bboxes.add(bbox);
      scores.add(score);
      landmarks.add(landmark);
    });
    // print(bboxes);
    // bboxes.forEach((element) {
    //   print(element);
    // });
    // print(scores);
    List<List<double>> re = sortDart.update(bboxes, landmarks, scores,3);
    for (int i = 0; i < re.length; i++) {
      print(
          "bbox[$i] = [ ${re[i][0]}, ${re[i][1]}, ${re[i][2]}, ${re[i][3]}, ${re[i][4]} ]");
    }
    frame_index++;
  }
  var endTime = DateTime.now().millisecondsSinceEpoch;
  var time_span = (endTime - startTime) / 1000;
  print("********************************");
  print("Total tracking took: ${time_span}s for ${total_frames} frames");
  print("FPS = ${total_frames / time_span}");
  print("********************************");
}
