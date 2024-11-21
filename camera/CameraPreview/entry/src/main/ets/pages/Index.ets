import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';

@Entry
@Component
struct Index {
  private mXComponentController: XComponentController = new XComponentController();
  private surfaceId: string = '';

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
        Stack({ alignContent: Alignment.Center }) {
          XComponent({
            id: 'preview-xid',
            type: XComponentType.SURFACE,
            libraryname: 'entry',
            controller: this.mXComponentController
          }).onLoad(async () => {
            this.surfaceId = this.mXComponentController.getXComponentSurfaceId();
          }).width('100%')
        }.width('100%').height('40%')

        Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
          Button('获取摄像头')
            .onClick(() => {
              if(!testNapi.ListCameras()) {
                this.toast("枚举摄像头失败")
              }
            })

          Button('创建输入')
            .onClick(() => {
              if(!testNapi.CreateInput(this.surfaceId)) {
                this.toast("创建摄像头输入失败")
              }
            })

          Button('创建会话')
            .onClick(() => {
              if(!testNapi.CreateSession()) {
                this.toast("创建摄像头会话失败")
              }
            })

          Button('删除会话')
            .onClick(() => {
              if(!testNapi.DestroySession()) {
                this.toast("删除摄像头会话失败")
              }
            })
        }

      }
      .width('100%')
    }
    .height('100%')
  }
}
