import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';

@Entry
@Component
struct Index {

  build() {
    Row() {
      Column() {
        Button('获取摄像头')
          .onClick(() => {
            testNapi.ListCameras()
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
