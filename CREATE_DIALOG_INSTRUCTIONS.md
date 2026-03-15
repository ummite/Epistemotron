# Creating the Simulation Configuration Dialog in Visual Studio

The `CSimConfigDlg` class is implemented but the dialog resource (`IDD_SIM_CONFIG_DLG = 313`) is missing from the `.rc` files. Follow these steps to create it.

## Required Controls

| Control ID | Type | Purpose |
|------------|------|---------|
| `IDC_EDIT_NUM_BODIES` | Edit Box | Number of bodies (integer, default: 4) |
| `IDC_EDIT_STEP_SIZE` | Edit Box | Time step in seconds (integer, default: 3600) |
| `IDC_EDIT_STEPS_PER_FRAME` | Edit Box | Steps per frame (integer, default: 1) |
| `IDC_CHECK_RANDOM` | Check Box | Use random positions (boolean) |
| `IDC_EDIT_RANDOM_RADIUS` | Edit Box | Random radius in km (double, default: 146000000) |
| `IDC_EDIT_DESCRIPTION` | Edit Box | Simulation description (string) |
| `IDC_BTN_RANDOMIZE` | Push Button | Auto-generate description |

## Step-by-Step Instructions

### 1. Open Resource Editor

1. Open `Epistemotron.sln` in Visual Studio
2. In Solution Explorer, expand `Resource Files`
3. Double-click `Epistemotron.rc` to open the Resource Editor
4. Right-click on `Dialog` folder → `Insert` → `Dialog`

### 2. Configure Dialog Properties

1. Select the new dialog (Dialog 1)
2. In Properties window:
   - **ID**: Change to `IDD_SIM_CONFIG_DLG` (313)
   - **Caption**: "Simulation Configuration"
   - **Style**: Dialog (default)
   - **Behavior Style**: None
   - **Help ID**: 0

### 3. Add Controls

Add the following controls in order (use Toolbox or Insert menu):

#### Group Box: "Simulation Parameters"
- **ID**: `IDC_GROUP_PARAMS` (optional, can be -1)
- **Position**: Top of dialog
- **Width**: Full dialog width

Inside the group box, add:

1. **Static Text**: "Number of Bodies:"
   - **ID**: -1 (no ID needed for labels)

2. **Edit Box** (for number of bodies)
   - **ID**: `IDC_EDIT_NUM_BODIES` (1001)
   - **Style**: Auto H-Scroll, Number
   - **Font**: Same as dialog default
   - **Initial text**: "4"

3. **Static Text**: "Time Step (seconds):"
   - **ID**: -1

4. **Edit Box** (for time step)
   - **ID**: `IDC_EDIT_STEP_SIZE` (1002)
   - **Style**: Auto H-Scroll, Number
   - **Initial text**: "3600"

5. **Static Text**: "Steps per Frame:"
   - **ID**: -1

6. **Edit Box** (for steps per frame)
   - **ID**: `IDC_EDIT_STEPS_PER_FRAME` (1003)
   - **Style**: Auto H-Scroll, Number
   - **Initial text**: "1"

#### Check Box: Random Positions

7. **Check Box**
   - **ID**: `IDC_CHECK_RANDOM` (1004)
   - **Text**: "Use random initial positions"
   - **Checked**: False (default)

#### Group Box: "Random Configuration" (enabled only when check is checked)

8. **Static Text**: "Random Radius (km):"
   - **ID**: -1

9. **Edit Box** (for random radius)
   - **ID**: `IDC_EDIT_RANDOM_RADIUS` (1005)
   - **Style**: Auto H-Scroll
   - **Initial text**: "146000000"

#### Group Box: "Description"

10. **Static Text**: "Description:"
    - **ID**: -1

11. **Edit Box** (for description, multi-line)
    - **ID**: `IDC_EDIT_DESCRIPTION` (1006)
    - **Style**: Auto H-Scroll, Multi-line, Want Return
    - **Initial text**: "Solar System Simulation"

#### Buttons

12. **Push Button** (Randomize)
    - **ID**: `IDC_BTN_RANDOMIZE` (1007)
    - **Text**: "Randomize Description"
    - **Position**: Above OK/Cancel buttons

13. **Push Button** (OK)
    - **ID**: `IDOK` (standard)
    - **Text**: "OK"
    - **Default Button**: Yes

14. **Push Button** (Cancel)
    - **ID**: `IDCANCEL` (standard)
    - **Text**: "Cancel"

### 4. Layout Suggestions

```
+--------------------------------------------------+
|  Simulation Configuration                        |
+--------------------------------------------------+
|  [Simulation Parameters]                         |
|  +--------------------------------------------+  |
|  | Number of Bodies:     [4        ]          |  |
|  | Time Step (seconds):  [3600     ]          |  |
|  | Steps per Frame:      [1        ]          |  |
|  +--------------------------------------------+  |
|                                                  |
|  [ ] Use random initial positions                |
|                                                  |
|  [Random Configuration]                          |
|  +--------------------------------------------+  |
|  | Random Radius (km): [146000000    ]         |  |
|  +--------------------------------------------+  |
|                                                  |
|  [Description]                                   |
|  +--------------------------------------------+  |
|  | Description:                                |  |
|  | [Solar System Simulation                  ] |  |
|  |                                            ] |  |
|  +--------------------------------------------+  |
|                                                  |
|              [Randomize Description]             |
|                                                  |
|                    [OK]    [Cancel]              |
+--------------------------------------------------+
```

### 5. Add Control IDs to Resource.h

After creating the dialog, Visual Studio should automatically add the IDs to `Resource.h`. Verify these entries exist:

```cpp
#define IDD_SIM_CONFIG_DLG            313
#define IDC_EDIT_NUM_BODIES           1001
#define IDC_EDIT_STEP_SIZE            1002
#define IDC_EDIT_STEPS_PER_FRAME      1003
#define IDC_CHECK_RANDOM              1004
#define IDC_EDIT_RANDOM_RADIUS        1005
#define IDC_EDIT_DESCRIPTION          1006
#define IDC_BTN_RANDOMIZE             1007
```

### 6. Build and Test

1. Save all files
2. Build the solution (Ctrl+Shift+B)
3. Run the application
4. Press Ctrl+Shift+C or use the Simulation menu to open the configuration dialog
5. Test all controls and the Randomize button

## Alternative: Programmatic Dialog Creation

If you prefer not to use the resource editor, you can create the dialog programmatically by overriding `Create()` in `CSimConfigDlg`. However, this is more complex and the resource editor approach is recommended for MFC applications.

## Notes

- The dialog uses MFC DDX (Dialog Data Exchange) for automatic data binding
- Input validation is implemented in `OnOK()` to prevent invalid simulation parameters
- The Randomize button auto-generates a description based on the configuration
