import { AbilityConstant, UIAbility, Want, Permissions } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'


const permissions: Array<Permissions> = [
  'ohos.permission.CAMERA',
];

function requestPermission(context: Context) {
  let atManager = abilityAccessCtrl.createAtManager();
  try {
    atManager.requestPermissionsFromUser(context, permissions).then((data) => {
      console.log(`audioCompressor request data->${JSON.stringify(data)}`);
    }).catch((err: object) => {
      console.log(`audioCompressor request data err ->${JSON.stringify(err)}`);
    })
  } catch (err) {
    console.log(`audioCompressor request permission catch err->${JSON.stringify(err)}`);
  }
}

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onCreate');
    requestPermission(this.context);
  }

  onDestroy(): void {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    windowStage.loadContent('pages/Index', (err) => {
      if (err.code) {
        hilog.error(0x0000, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      hilog.info(0x0000, 'testTag', 'Succeeded in loading the content.');
    });
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    // Ability has brought to foreground
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
    // Ability has back to background
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onBackground');
  }
};
