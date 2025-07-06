# Using the IconButton and FilledColorButton Components

This document shows how to use the new reusable button components in your Volt editor codebase.

## IconButton Component

The `IconButton` component provides VS Code-like icon buttons with SVG support and high-DPI rendering.

### Basic Usage

```cpp
#include "ui/components/IconButton.h"

// Simple icon button
IconButton *button = new IconButton(":/icons/my_icon.svg", this);

// With tooltip
IconButton *button = new IconButton(":/icons/my_icon.svg", "My Tooltip", this);

// Set icon after creation
IconButton *button = new IconButton(this);
button->setIconPath(":/icons/my_icon.svg");
button->setToolTip("My Tooltip");
```

### Customization

```cpp
// Custom sizes
button->setIconSize(QSize(20, 20));  // Icon size
button->setButtonSize(QSize(28, 28)); // Button size

// Custom icon color (overrides theme)
button->setIconColor(QColor("#FF5733"));

// Apply VS Code styling (default)
button->applyVSCodeStyle();

// Apply custom styling
button->applyCustomStyle("QPushButton { background: red; }");

// Refresh icon (useful after theme changes)
button->refreshIcon();
```

### Connecting Signals

```cpp
connect(button, &IconButton::clicked, this, &MyClass::onButtonClicked);
```

## FilledColorButton Component

The `FilledColorButton` component provides Material Design-like filled buttons with various styles.

### Basic Usage

```cpp
#include "ui/components/FilledColorButton.h"

// Simple button with primary style
FilledColorButton *button = new FilledColorButton("Save", this);

// With specific style
FilledColorButton *button = new FilledColorButton("Delete", FilledColorButton::Danger, this);
```

### Available Styles

```cpp
// Predefined styles
FilledColorButton::Primary   // Blue (default)
FilledColorButton::Secondary // Gray
FilledColorButton::Accent    // Teal
FilledColorButton::Success   // Green
FilledColorButton::Warning   // Yellow
FilledColorButton::Danger    // Red
FilledColorButton::Custom    // User-defined colors
```

### Customization

```cpp
// Change style
button->setButtonStyle(FilledColorButton::Success);

// Custom colors
button->setCustomColors(
    QColor("#FF5733"),  // background
    QColor("#FFFFFF"),  // text
    QColor("#FF7961"),  // hover (optional)
    QColor("#C62828")   // pressed (optional)
);

// Individual color changes
button->setBackgroundColor(QColor("#007ACC"));
button->setTextColor(QColor("#FFFFFF"));
button->setHoverColor(QColor("#1E88E5"));
button->setPressedColor(QColor("#0D47A1"));

// Border radius
button->setBorderRadius(8);

// Refresh styling
button->refreshStyle();
```

### Connecting Signals

```cpp
connect(button, &FilledColorButton::clicked, this, &MyClass::onButtonClicked);
```

## Theme Integration

Both components automatically respond to theme changes:

```cpp
// Manual theme application (usually not needed)
iconButton->applyTheme();
filledButton->applyTheme();
```

## Example: Creating a Toolbar

```cpp
void MyWidget::createToolbar()
{
    QHBoxLayout *toolbar = new QHBoxLayout();
    
    // Icon buttons
    IconButton *newFile = new IconButton(":/icons/new_file.svg", "New File", this);
    IconButton *openFile = new IconButton(":/icons/open_file.svg", "Open File", this);
    IconButton *saveFile = new IconButton(":/icons/save_file.svg", "Save File", this);
    
    // Filled buttons
    FilledColorButton *compile = new FilledColorButton("Compile", FilledColorButton::Primary, this);
    FilledColorButton *run = new FilledColorButton("Run", FilledColorButton::Success, this);
    FilledColorButton *stop = new FilledColorButton("Stop", FilledColorButton::Danger, this);
    
    // Add to toolbar
    toolbar->addWidget(newFile);
    toolbar->addWidget(openFile);
    toolbar->addWidget(saveFile);
    toolbar->addStretch();
    toolbar->addWidget(compile);
    toolbar->addWidget(run);
    toolbar->addWidget(stop);
    
    // Connect signals
    connect(newFile, &IconButton::clicked, this, &MyWidget::newFile);
    connect(openFile, &IconButton::clicked, this, &MyWidget::openFile);
    connect(saveFile, &IconButton::clicked, this, &MyWidget::saveFile);
    connect(compile, &FilledColorButton::clicked, this, &MyWidget::compile);
    connect(run, &FilledColorButton::clicked, this, &MyWidget::run);
    connect(stop, &FilledColorButton::clicked, this, &MyWidget::stop);
}
```

## Theme Configuration

Add these colors to your theme JSON files for proper styling:

```json
{
  "explorer": {
    "foreground": "#CCCCCC",
    "hoverBackground": "#2A2D2E"
  },
  "button": {
    "primary": {
      "background": "#007ACC",
      "foreground": "#FFFFFF",
      "hoverBackground": "#1E88E5",
      "pressedBackground": "#0D47A1"
    },
    "secondary": {
      "background": "#5A5A5A",
      "foreground": "#FFFFFF",
      "hoverBackground": "#757575",
      "pressedBackground": "#424242"
    },
    "accent": {
      "background": "#14A085",
      "foreground": "#FFFFFF",
      "hoverBackground": "#26A69A",
      "pressedBackground": "#00695C"
    },
    "success": {
      "background": "#28A745",
      "foreground": "#FFFFFF",
      "hoverBackground": "#34CE57",
      "pressedBackground": "#1E7E34"
    },
    "warning": {
      "background": "#FFC107",
      "foreground": "#212529",
      "hoverBackground": "#FFD54F",
      "pressedBackground": "#FF8F00"
    },
    "danger": {
      "background": "#DC3545",
      "foreground": "#FFFFFF",
      "hoverBackground": "#E57373",
      "pressedBackground": "#C62828"
    }
  }
}
```
