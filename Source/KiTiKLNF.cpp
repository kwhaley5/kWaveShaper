/*
  ==============================================================================

    KiTiKLNF.cpp
    Created: 26 Jul 2023 6:20:24pm
    Author:  kylew

  ==============================================================================
*/

#include "KiTiKLNF.h"

void Laf::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    using namespace juce;

    auto unfill = Colour(15u, 15u, 15u);
    auto fill = Colour(64u, 194u, 230u);

    auto boundsFull = Rectangle<int>(x, y, width, height).toFloat();
    auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin(8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    auto rootTwo = MathConstants<float>::sqrt2;

    Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true);

    g.setColour(unfill);
    g.strokePath(backgroundArc, PathStrokeType(lineW / 2, PathStrokeType::curved, PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            toAngle,
            true);

        g.setColour(fill);
        g.strokePath(valueArc, PathStrokeType(lineW / 2, PathStrokeType::curved, PathStrokeType::rounded));
    }

    //make circle with gradient
    float radialBlur = radius * 2.5;

    auto grad = ColourGradient::ColourGradient(Colour(186u, 34u, 34u), bounds.getCentreX(), bounds.getCentreY(), Colours::black, radialBlur, radialBlur, true);

    g.setGradientFill(grad);
    g.fillRoundedRectangle(boundsFull.getCentreX() - (radius * rootTwo / 2), boundsFull.getCentreY() - (radius * rootTwo / 2), radius * rootTwo, radius * rootTwo, radius * .7);

    //add circle around dial
    g.setColour(Colours::lightslategrey);
    g.drawRoundedRectangle(boundsFull.getCentreX() - (radius * rootTwo / 2), boundsFull.getCentreY() - (radius * rootTwo / 2), radius * rootTwo, radius * rootTwo, radius * .7, 1.5f);

    //make dial line
    g.setColour(Colours::whitesmoke);
    Point<float> thumbPoint(bounds.getCentreX() + radius / rootTwo * std::cos(toAngle - MathConstants<float>::halfPi), //This is one is farthest from center.
        bounds.getCentreY() + radius / rootTwo * std::sin(toAngle - MathConstants<float>::halfPi));

    if (slider.getComponentID() == "LFO")
        rootTwo = 2;

    Point<float> shortLine(bounds.getCentreX() + (arcRadius - (arcRadius / rootTwo)) * std::cos(toAngle - MathConstants<float>::halfPi), //This one is closer to the center
        bounds.getCentreY() + (arcRadius - (arcRadius / rootTwo)) * std::sin(toAngle - MathConstants<float>::halfPi));

    g.drawLine(shortLine.getX(), shortLine.getY(), thumbPoint.getX(), thumbPoint.getY(), lineW / 2);

    //Add text Values
    if (slider.getComponentID() == "Filter")
    {
        auto fontSize = 15.f;
        auto str = String('none');
        auto value = slider.getValue();

        //if(slider.getName() == ""

        if (slider.getName() == "Drive" || slider.getName() == "Gain")
        {
            str = String(value);
        }
        else if (slider.getName() == "Rate" || slider.getName() == "Frequency" || (slider.getName() == "Depth" && slider.getTitle() == "fmDepth"))
        {
            str = String(value);
            str.append(" Hz", 5);
        }
        else if (value <= 1) {
            value *= 100;
            str = String(value);
            str.append("%", 3);
        }
        else {
            str = String(value);
            str.append(" ms", 5);
        }

        auto strWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<float> r;
        r.setBottom(boundsFull.getBottom() - 10);
        r.setLeft(boundsFull.getCentre().getX() - strWidth);
        r.setRight(boundsFull.getCentre().getX() + strWidth);
        r.setTop(boundsFull.getBottom() - 30);

        g.setColour(Colours::whitesmoke);
        g.setFont(fontSize);
        g.drawFittedText(str, r.getX(), r.getY(), r.getWidth(), r.getHeight(), juce::Justification::centred, 1);

        auto name = slider.getName();
        strWidth = g.getCurrentFont().getStringWidth(name);
        r.setTop(0);
        r.setLeft(boundsFull.getCentre().getX() - strWidth);
        r.setRight(boundsFull.getCentre().getX() + strWidth);
        r.setBottom(20);
        g.setColour(Colours::whitesmoke);
        g.drawFittedText(name, r.getX(), r.getY(), r.getWidth(), r.getHeight(), juce::Justification::centred, 1);
    }
    else if (slider.getComponentID() == "LFO")
    {
        auto fontSize = 14.f;
        auto str = String('none');
        auto value = slider.getValue();

        value *= 100;
        str = String(value);
        str.append("%", 3);

        auto strWidth = g.getCurrentFont().getStringWidth(str);

        Rectangle<float> r;
        r.setBottom(boundsFull.getCentreY() + 10);
        r.setLeft(boundsFull.getCentre().getX() - strWidth);
        r.setRight(boundsFull.getCentre().getX() + strWidth);
        r.setTop(boundsFull.getCentreY() - 10);

        g.setColour(Colours::whitesmoke);
        g.setFont(fontSize);
        g.drawFittedText(str, r.getX(), r.getY(), r.getWidth(), r.getHeight(), juce::Justification::centred, 1);
    }

}

void Laf::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{

    if (button.getComponentID() == "Power")
    {
        juce::Path powerButton;

        auto bounds = button.getLocalBounds().reduced(2);
        auto size = juce::jmin(bounds.getWidth(), bounds.getHeight()) - 6;
        auto r = bounds.withSizeKeepingCentre(size, size).toFloat();

        float ang = 30.f;

        size -= 12;

        powerButton.addCentredArc(r.getCentreX(), r.getCentreY(), size * .5, size * .5, 0.f, juce::degreesToRadians(ang), juce::degreesToRadians(360 - ang), true);

        powerButton.startNewSubPath(r.getCentreX(), r.getY() + 3);
        powerButton.lineTo(r.getCentre());

        juce::PathStrokeType pst(2.f, juce::PathStrokeType::JointStyle::curved);

        auto color = button.getToggleState() ? juce::Colours::dimgrey : juce::Colours::white;

        g.setColour(color);
        g.strokePath(powerButton, pst);
        g.drawEllipse(r, 2);

    }
    else 
    {
        auto fontSize = juce::jmin(15.0f, (float)button.getHeight());
        auto getFont = g.getCurrentFont();
        auto rectWidth = getFont.getStringWidthFloat(button.getButtonText());

        juce::Rectangle<float> r;

        //Tweak this when you make your cutsom LNF headers and cpp files
        r.setSize(juce::jmax(20.f, rectWidth * 1.5f), fontSize * 1.1);
        r.translate((button.getWidth() / 2 - rectWidth * .75f), (button.getHeight() / 2 - fontSize / 2 - 1.f));


        auto color = button.getToggleState() ? juce::Colour(64u, 194u, 230u) : juce::Colours::dimgrey;
        g.setColour(color);
        g.fillRoundedRectangle(r, fontSize / 4);

        auto bounds = r.toFloat();
        g.setColour(juce::Colours::black);
        g.drawRoundedRectangle(bounds.getCentreX() - bounds.getWidth() / 2, bounds.getCentreY() - bounds.getHeight() / 2, bounds.getWidth(), bounds.getHeight(), fontSize / 4, 2);

        if (!button.isEnabled())
            g.setOpacity(0.5f);

        g.setColour(button.findColour(juce::ToggleButton::textColourId));
        g.setFont(fontSize);
        g.drawFittedText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 10);
    }

}

void Laf::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    using namespace juce;
    auto boundsFull = Rectangle<int>(x, y, width, height).toFloat();

    if (slider.isBar())
    {
        g.setColour(slider.findColour(Slider::trackColourId));
        g.fillRect(slider.isHorizontal() ? Rectangle<float>(static_cast<float> (x), (float)y + 0.5f, sliderPos - (float)x, (float)height - 1.0f)
            : Rectangle<float>((float)x + 0.5f, sliderPos, (float)width - 1.0f, (float)y + ((float)height - sliderPos)));
    }
    else
    {
        auto isTwoVal = (style == Slider::SliderStyle::TwoValueVertical || style == Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

        Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
            slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

        Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
            slider.isHorizontal() ? startPoint.y : (float)y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(Colours::black);
        g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : (float)width * 0.5f,
                         slider.isHorizontal() ? (float)height * 0.5f : minSliderPos };

            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : (float)width * 0.5f,
                               slider.isHorizontal() ? (float)height * 0.5f : sliderPos };

            maxPoint = { slider.isHorizontal() ? maxSliderPos : (float)width * 0.5f,
                         slider.isHorizontal() ? (float)height * 0.5f : maxSliderPos };
        }
        else
        {
            auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
            auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        auto thumbWidth = getSliderThumbRadius(slider);
 
        valueTrack.startNewSubPath(minPoint);
        valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
        g.setColour(Colour(64u, 194u, 230u));
        g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        if (!isTwoVal)
        {
            auto name = slider.getName();
            auto value = static_cast<String>(slider.getValue());
            auto thumb = slider.getThumbBeingDragged();
            
            auto font = g.getCurrentFont();
            auto newWidth = font.getStringWidth(name);
            auto thumbRect = Rectangle<float>(static_cast<float> (newWidth + 5), static_cast<float> (thumbWidth + 5)).withCentre(isThreeVal ? thumbPoint : maxPoint);
            auto grad = ColourGradient::ColourGradient(Colour(186u, 34u, 34u), thumbRect.getCentreX(), thumbRect.getCentreY(), Colours::black, thumbRect.getWidth() / 2, thumbRect.getHeight() / 2, true);
            g.setGradientFill(grad);
            
            g.fillRoundedRectangle(thumbRect, 2);
            g.setColour(juce::Colours::whitesmoke);
            g.drawText(thumb ? name : value, thumbRect, Justification::centred, 1);
        }

        if (isTwoVal || isThreeVal)
        {
            auto sr = jmin(trackWidth, (slider.isHorizontal() ? (float)height : (float)width) * 0.4f);
            auto pointerColour = slider.findColour(Slider::thumbColourId);

            if (slider.isHorizontal())
            {
                drawPointer(g, minSliderPos - sr,
                    jmax(0.0f, (float)y + (float)height * 0.5f - trackWidth * 2.0f),
                    trackWidth * 2.0f, pointerColour, 2);

                drawPointer(g, maxSliderPos - trackWidth,
                    jmin((float)(y + height) - trackWidth * 2.0f, (float)y + (float)height * 0.5f),
                    trackWidth * 2.0f, pointerColour, 4);
            }
            else
            {
                drawPointer(g, jmax(0.0f, (float)x + (float)width * 0.5f - trackWidth * 2.0f),
                    minSliderPos - trackWidth,
                    trackWidth * 2.0f, pointerColour, 1);

                drawPointer(g, jmin((float)(x + width) - trackWidth * 2.0f, (float)x + (float)width * 0.5f), maxSliderPos - sr,
                    trackWidth * 2.0f, pointerColour, 3);
            }
        }
    }
}

void Laf::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box)
{
    using namespace juce;

    auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(box.findColour(ComboBox::backgroundColourId));
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

    g.setColour(juce::Colours::black.withAlpha(.7f));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

    Rectangle<int> arrowZone(width - 30, 0, 20, height);
    Path path;
    path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
    path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
    path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

    g.setColour(box.findColour(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
    g.strokePath(path, PathStrokeType(2.0f));
}

void Laf::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour)
{
    using namespace juce;

    if (isSeparator)
    {
        auto r = area.reduced(5, 0);
        r.removeFromTop(roundToInt(((float)r.getHeight() * 0.5f) - 0.5f));

        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillRect(r);
    }
    else
    {
        auto textColour =  findColour(PopupMenu::textColourId);
        auto background =  Colours::black.withAlpha(0.3f);

        g.setColour(background);

        auto r = area.reduced(1);
        g.fillRect(r);

        if (isHighlighted && isActive)
        {
            g.setColour(findColour(PopupMenu::highlightedBackgroundColourId));
            g.fillRect(r);

            g.setColour(findColour(PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour(textColour.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
        }

        r.reduce(jmin(5, area.getWidth() / 20), 0);

        auto font = getPopupMenuFont();

        auto maxFontHeight = (float)r.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight)
            font.setHeight(maxFontHeight);

        g.setFont(font);

        if (hasSubMenu)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            auto x = static_cast<float> (r.removeFromRight((int)arrowH).getX());
            auto halfH = static_cast<float> (r.getCentreY());

            Path path;
            path.startNewSubPath(x, halfH - arrowH * 0.5f);
            path.lineTo(x + arrowH * 0.6f, halfH);
            path.lineTo(x, halfH + arrowH * 0.5f);

            g.strokePath(path, PathStrokeType(2.0f));
        }

        r.removeFromRight(3);
        g.drawFittedText(text, r, Justification::centredLeft, 1);

        if (shortcutKeyText.isNotEmpty())
        {
            auto f2 = font;
            f2.setHeight(f2.getHeight() * 0.75f);
            f2.setHorizontalScale(0.95f);
            g.setFont(f2);

            g.drawText(shortcutKeyText, r, Justification::centredRight, true);
        }
    }
}

void Laf::LevelMeter::paint(juce::Graphics& g)
{
    {
        using namespace juce;

        //shapes the meters. May be a bit inefficeint, not sure the best way to move this stuff around, but it is there.
        auto bounds = getLocalBounds().toFloat();
        bounds = bounds.removeFromLeft(bounds.getWidth() * .75);
        bounds = bounds.removeFromRight(bounds.getWidth() * .66);
        bounds = bounds.removeFromTop(bounds.getHeight() * .9);
        bounds = bounds.removeFromBottom(bounds.getHeight() * .88);

        //get our base rectangle
        g.setColour(Colours::black);
        g.fillRoundedRectangle(bounds, 5.f);

        //get gradient
        auto gradient = ColourGradient::ColourGradient(Colours::green, bounds.getBottomLeft(), Colours::red, bounds.getTopLeft(), false);
        gradient.addColour(.5f, Colours::yellow);
        g.setGradientFill(gradient);

        //Show gradient
        auto levelMeterFill = jmap(level, -60.f, +6.f, 0.f, static_cast<float>(bounds.getHeight()));
        g.fillRoundedRectangle(bounds.removeFromBottom(levelMeterFill), 5.f);
    }
}