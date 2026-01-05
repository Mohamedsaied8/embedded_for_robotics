#!/usr/bin/env python3
"""
Generate ODP presentation for Quadrature Encoder explanation
"""

from odf.opendocument import OpenDocumentPresentation
from odf.style import Style, MasterPage, PageLayout, PageLayoutProperties
from odf.style import TextProperties, GraphicProperties, ParagraphProperties, DrawingPageProperties
from odf.draw import Page, Frame, TextBox, Image
from odf.text import P, Span
from odf import dc

# Create presentation document
doc = OpenDocumentPresentation()

# Page layout
pagelayout = PageLayout(name="MyLayout")
doc.automaticstyles.addElement(pagelayout)
pagelayout.addElement(PageLayoutProperties(
    margin="0cm", 
    pagewidth="28cm", 
    pageheight="21cm",
    printorientation="landscape"
))

# Master page
masterpage = MasterPage(name="Default", pagelayoutname=pagelayout)
doc.masterstyles.addElement(masterpage)

# Title style
titlestyle = Style(name="TitleStyle", family="presentation")
titlestyle.addElement(TextProperties(
    fontsize="44pt",
    fontweight="bold",
    color="#1a5276"
))
titlestyle.addElement(ParagraphProperties(textalign="center"))
doc.automaticstyles.addElement(titlestyle)

# Subtitle style
subtitlestyle = Style(name="SubtitleStyle", family="presentation")
subtitlestyle.addElement(TextProperties(
    fontsize="28pt",
    color="#2e86ab"
))
subtitlestyle.addElement(ParagraphProperties(textalign="center"))
doc.automaticstyles.addElement(subtitlestyle)

# Content style
contentstyle = Style(name="ContentStyle", family="presentation")
contentstyle.addElement(TextProperties(fontsize="24pt", color="#2c3e50"))
doc.automaticstyles.addElement(contentstyle)

# Small content style
smallstyle = Style(name="SmallStyle", family="presentation")
smallstyle.addElement(TextProperties(fontsize="18pt", color="#34495e"))
doc.automaticstyles.addElement(smallstyle)

# Code style
codestyle = Style(name="CodeStyle", family="presentation")
codestyle.addElement(TextProperties(fontsize="16pt", fontfamily="Monospace", color="#27ae60"))
doc.automaticstyles.addElement(codestyle)

# Header style
headerstyle = Style(name="HeaderStyle", family="presentation")
headerstyle.addElement(TextProperties(fontsize="32pt", fontweight="bold", color="#e74c3c"))
doc.automaticstyles.addElement(headerstyle)

def add_slide(doc, title, content_lines, is_title_slide=False):
    """Add a slide to the presentation"""
    page = Page(stylename=masterpage, masterpagename=masterpage)
    doc.presentation.addElement(page)
    
    # Title frame
    titleframe = Frame(
        stylename=titlestyle if is_title_slide else headerstyle,
        width="26cm",
        height="3cm" if is_title_slide else "2.5cm",
        x="1cm",
        y="1cm" if is_title_slide else "0.5cm"
    )
    page.addElement(titleframe)
    textbox = TextBox()
    titleframe.addElement(textbox)
    p = P(stylename=titlestyle if is_title_slide else headerstyle)
    p.addText(title)
    textbox.addElement(p)
    
    # Content frame
    if content_lines:
        contentframe = Frame(
            stylename=contentstyle,
            width="26cm",
            height="15cm",
            x="1cm",
            y="4cm" if is_title_slide else "3.5cm"
        )
        page.addElement(contentframe)
        textbox = TextBox()
        contentframe.addElement(textbox)
        
        for line in content_lines:
            if line.startswith("##"):
                # Subheader
                p = P(stylename=subtitlestyle)
                p.addText(line[2:].strip())
            elif line.startswith("```"):
                # Code block
                p = P(stylename=codestyle)
                p.addText(line[3:].strip())
            elif line.startswith("•"):
                # Bullet point
                p = P(stylename=contentstyle)
                p.addText(line)
            else:
                p = P(stylename=smallstyle)
                p.addText(line)
            textbox.addElement(p)
    
    return page

# ============== SLIDES ==============

# Slide 1: Title
add_slide(doc, "Quadrature Encoder Interface", [
    "",
    "STM32F103 Bare-Metal Implementation",
    "",
    "Direct Register Access",
    "No CMSIS | No HAL",
    "",
    "Using TIM2 Hardware Encoder Mode"
], is_title_slide=True)

# Slide 2: What is a Quadrature Encoder?
add_slide(doc, "What is a Quadrature Encoder?", [
    "## Definition",
    "• A rotary encoder that outputs two signals (A and B) 90° out of phase",
    "• Used for measuring: Position, Speed, Direction",
    "",
    "## Key Features",
    "• Two output channels: A and B",
    "• 90° phase difference allows direction detection",
    "• Hardware decoding available in STM32 timers",
    "",
    "## Applications",
    "• Motor position feedback",
    "• Robotic joints",
    "• CNC machines",
    "• Volume knobs"
])

# Slide 3: Quadrature Signals
add_slide(doc, "Understanding Quadrature Signals", [
    "## Signal Pattern",
    "",
    "  Channel A: ___/‾‾‾\\___/‾‾‾\\___/‾‾‾\\___",
    "  Channel B: _/‾‾‾\\___/‾‾‾\\___/‾‾‾\\___/‾",
    "",
    "## Direction Detection",
    "• Clockwise:        A leads B (A rises before B)",
    "• Counter-clockwise: B leads A (B rises before A)",
    "",
    "## Resolution",
    "• 1x mode: Count on A rising edge only",
    "• 2x mode: Count on A rising and falling",
    "• 4x mode: Count on both A and B edges (highest resolution)"
])

# Slide 4: STM32 Timer Encoder Mode
add_slide(doc, "STM32 Timer Encoder Mode", [
    "## Hardware Features",
    "• TIM2, TIM3, TIM4 support encoder interface",
    "• Automatic counting (no software intervention)",
    "• Direction handled by hardware",
    "• 16-bit or 32-bit counter (TIM2 is 32-bit)",
    "",
    "## Encoder Mode Selection (SMCR register)",
    "• Mode 1 (SMS=001): Count on TI1 edges only",
    "• Mode 2 (SMS=010): Count on TI2 edges only",
    "• Mode 3 (SMS=011): Count on both TI1 and TI2 edges",
    "",
    "## Pin Mapping",
    "• TIM2_CH1 → PA0 (Encoder A)",
    "• TIM2_CH2 → PA1 (Encoder B)"
])

# Slide 5: Register Overview
add_slide(doc, "Key Registers for Encoder Mode", [
    "## RCC (Clock Control)",
    "• APB1ENR: Enable TIM2 clock",
    "• APB2ENR: Enable GPIOA clock",
    "",
    "## GPIO Configuration",
    "• GPIOA_CRL: Configure PA0, PA1 as inputs",
    "",
    "## Timer Registers",
    "• TIM2_CR1:   Control (enable/disable)",
    "• TIM2_SMCR:  Slave mode (encoder mode selection)",
    "• TIM2_CCMR1: Capture/compare (input mapping)",
    "• TIM2_CCER:  Enable channels",
    "• TIM2_ARR:   Auto-reload (counter max)",
    "• TIM2_CNT:   Counter value (encoder position!)"
])

# Slide 6: Step 1 - Enable Clocks
add_slide(doc, "Step 1: Enable Peripheral Clocks", [
    "## RCC_APB1ENR Register (0x4002101C)",
    "",
    "```RCC->APB1ENR |= (1 << 0);  // Bit 0 = TIM2EN",
    "",
    "• Enables TIM2 peripheral clock",
    "• Without this, timer registers won't respond",
    "",
    "## RCC_APB2ENR Register (0x40021018)",
    "",
    "```RCC->APB2ENR |= (1 << 2);  // Bit 2 = IOPAEN",
    "",
    "• Enables GPIOA clock",
    "• Required for PA0 and PA1 to function"
])

# Slide 7: Step 2 - Configure GPIO
add_slide(doc, "Step 2: Configure GPIO Pins", [
    "## GPIOA_CRL Register (0x40010800)",
    "",
    "• PA0 [bits 3:0]: CNF=01 (floating), MODE=00 (input)",
    "• PA1 [bits 7:4]: CNF=01 (floating), MODE=00 (input)",
    "",
    "```GPIOA->CRL = 0x00000044;",
    "",
    "## GPIO Mode/CNF Bits",
    "• MODE = 00: Input mode",
    "• CNF = 01: Floating input (default for timer)",
    "• CNF = 10: Input with pull-up/pull-down",
    "",
    "## Alternative: Input with Pull-up",
    "```GPIOA->CRL = 0x00000088;  // CNF=10",
    "```GPIOA->ODR |= 0x03;       // Enable pull-ups"
])

# Slide 8: Step 3 - Configure Encoder Mode
add_slide(doc, "Step 3: Set Encoder Mode (SMCR)", [
    "## TIM2_SMCR Register (0x40000008)",
    "",
    "• SMS[2:0] bits select slave mode",
    "",
    "## Encoder Mode Values",
    "• SMS = 001: Encoder mode 1 (TI1 edges)",
    "• SMS = 010: Encoder mode 2 (TI2 edges)",
    "• SMS = 011: Encoder mode 3 (both edges) ← Best!",
    "",
    "```TIM2->SMCR = (TIM2->SMCR & ~0x07) | 0x03;",
    "",
    "## Why Mode 3?",
    "• Counts on all edges → 4x resolution",
    "• Example: 100 PPR encoder → 400 counts/rev"
])

# Slide 9: Step 4 - Configure Input Channels
add_slide(doc, "Step 4: Map Input Channels (CCMR1)", [
    "## TIM2_CCMR1 Register (0x40000018)",
    "",
    "• CC1S[1:0]: Channel 1 input selection",
    "• CC2S[1:0]: Channel 2 input selection",
    "",
    "## Configuration",
    "• CC1S = 01: IC1 mapped on TI1 (PA0)",
    "• CC2S = 01: IC2 mapped on TI2 (PA1)",
    "",
    "```TIM2->CCMR1 = 0x0101;",
    "",
    "## Optional: Input Filter (IC1F, IC2F)",
    "• Bits [7:4] for IC1, Bits [15:12] for IC2",
    "• Higher value = more filtering (debouncing)",
    "```TIM2->CCMR1 = 0x3131;  // Filter N=3"
])

# Slide 10: Step 5 - Enable Channels
add_slide(doc, "Step 5: Enable Channels (CCER)", [
    "## TIM2_CCER Register (0x40000020)",
    "",
    "• CC1E: Enable channel 1 capture",
    "• CC2E: Enable channel 2 capture",
    "• CC1P/CC2P: Polarity (0=non-inverted, 1=inverted)",
    "",
    "## Configuration",
    "",
    "```TIM2->CCER = 0x0011;  // CC1E=1, CC2E=1",
    "",
    "## Inverting Direction",
    "• If encoder counts wrong direction:",
    "```TIM2->CCER = 0x0013;  // Invert CC1 polarity",
    "",
    "• Or swap A/B wires physically"
])

# Slide 11: Step 6 - Set Counter Range
add_slide(doc, "Step 6: Configure Counter Range", [
    "## TIM2_ARR Register (0x4000002C)",
    "",
    "• Sets maximum counter value",
    "• Counter wraps around at ARR value",
    "• TIM2 is 32-bit capable!",
    "",
    "```TIM2->ARR = 0xFFFFFFFF;  // Full 32-bit range",
    "",
    "## TIM2_PSC Register (0x40000028)",
    "• Prescaler (usually 0 for encoders)",
    "",
    "```TIM2->PSC = 0;  // No prescaling",
    "",
    "## TIM2_CNT Register (0x40000024)",
    "• This is where you READ the encoder position!",
    "```int32_t position = TIM2->CNT;"
])

# Slide 12: Step 7 - Enable Timer
add_slide(doc, "Step 7: Enable the Timer", [
    "## TIM2_CR1 Register (0x40000000)",
    "",
    "• CEN (bit 0): Counter enable",
    "• Other bits usually left at default",
    "",
    "## Enable Counter",
    "",
    "```TIM2->CR1 |= 0x01;  // Set CEN bit",
    "",
    "## That's it!",
    "• Timer now counts automatically",
    "• No interrupts needed",
    "• Just read TIM2->CNT when you need position",
    "",
    "## Reset Counter",
    "```TIM2->CNT = 0;  // Reset to zero"
])

# Slide 13: Complete Code
add_slide(doc, "Complete Initialization Code", [
    "```// 1. Enable clocks",
    "```RCC->APB1ENR |= (1 << 0);   // TIM2",
    "```RCC->APB2ENR |= (1 << 2);   // GPIOA",
    "",
    "```// 2. Configure GPIO (PA0, PA1 floating input)",
    "```GPIOA->CRL = 0x00000044;",
    "",
    "```// 3. Encoder mode 3 (both edges)",
    "```TIM2->SMCR = 0x0003;",
    "",
    "```// 4. Map TI1 and TI2",
    "```TIM2->CCMR1 = 0x0101;",
    "",
    "```// 5. Enable channels",
    "```TIM2->CCER = 0x0011;",
    "",
    "```// 6. Full 32-bit range",
    "```TIM2->ARR = 0xFFFFFFFF;",
    "",
    "```// 7. Enable timer",
    "```TIM2->CR1 = 0x0001;"
])

# Slide 14: Reading Encoder
add_slide(doc, "Reading the Encoder Position", [
    "## Get Current Count",
    "",
    "```int32_t Encoder_GetCount(void) {",
    "```    return (int32_t)TIM2->CNT;",
    "```}",
    "",
    "## Reset to Zero",
    "",
    "```void Encoder_Reset(void) {",
    "```    TIM2->CNT = 0;",
    "```}",
    "",
    "## Calculate Velocity (optional)",
    "",
    "```// In a timer interrupt (every 10ms):",
    "```int32_t delta = TIM2->CNT - last_count;",
    "```float rpm = delta * 6000.0 / (PPR * 4);",
    "```last_count = TIM2->CNT;"
])

# Slide 15: Hardware Wiring
add_slide(doc, "Hardware Wiring Diagram", [
    "## Connections",
    "",
    "  Encoder          STM32F103C8T6",
    "  --------         -------------",
    "    A    ────────►  PA0 (TIM2_CH1)",
    "    B    ────────►  PA1 (TIM2_CH2)",
    "   GND   ────────►  GND",
    "   VCC   ────────►  3.3V or 5V",
    "",
    "## Notes",
    "• Most encoders have internal pull-ups",
    "• Add 10kΩ external pull-ups if needed",
    "• Check encoder voltage compatibility",
    "• Some encoders have index (Z) output for homing"
])

# Slide 16: Summary
add_slide(doc, "Summary", [
    "## Key Points",
    "• STM32 has hardware encoder interface",
    "• TIM2/3/4 support encoder mode",
    "• Just 7 register writes to configure",
    "• Counter updates automatically - no CPU load!",
    "",
    "## Register Sequence",
    "1. RCC→APB1ENR: Enable TIM2 clock",
    "2. RCC→APB2ENR: Enable GPIOA clock",
    "3. GPIOA→CRL: Configure PA0, PA1 as inputs",
    "4. TIM2→SMCR: Set encoder mode 3",
    "5. TIM2→CCMR1: Map inputs",
    "6. TIM2→CCER: Enable channels",
    "7. TIM2→ARR: Set counter range",
    "8. TIM2→CR1: Enable timer",
    "",
    "## Read Position: TIM2→CNT"
])

# Save the document
output_path = "/media/mohamed/Robotics1/repos/embedded_for_robotics/quadrature_encoder_baremetal/Quadrature_Encoder_Slides.odp"
doc.save(output_path)
print(f"Presentation saved to: {output_path}")
