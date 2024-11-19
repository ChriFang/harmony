import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';

@Entry
@Component
struct Index {
  private toast(msg: string) {
    AlertDialog.show(
      {
        title: '提示',
        message: msg,
        autoCancel: false,
        alignment: DialogAlignment.Center,
        offset: { dx: 0, dy: -20 },
        gridCount: 3,
        confirm: {
          value: '确定',
          action: () => {
            console.info('Button-clicking callback')
          }
        }
      }
    )
  }

  build() {
    Row() {
      Column() {
        Button('获取摄像头')
          .onClick(() => {
            if(!testNapi.ListCameras()) {
              this.toast("枚举摄像头失败")
            }
          })

        Button('创建输入')
          .onClick(() => {
            if(!testNapi.CreateInput()) {
              this.toast("创建摄像头输入失败")
            }
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
