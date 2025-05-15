#include <iostream>
#include "mylib.hpp"
#include <map>
#include <algorithm>
using namespace std;

class Note
{
public:
    string noteName;
    int octave;

    Note(string noteName, int octave)
    {
        this->noteName = noteName;
        this->octave = octave;
    }

    Note() : noteName("ERR"), octave(-1) {}

    static Note ErrorNote()
    {
        return Note("ERR", -1);
    }

    bool isError() const
    {
        return noteName == "ERR" && octave == -1;
    }
    string info()
    {
        return noteName + to_string(octave);
    }
};
class MusicCalculator
{

public:
    static Note transpose(Note note, int interval, bool accidentalType)
    {
        vector<string> chromaticScaleSharps = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
        vector<string> chromaticScaleFlats = {"A", "Bb", "B", "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab"};
        Note transposedNote = note;
        vector<string> scaleToUse = chromaticScaleFlats;
        if (!accidentalType)
        {
            scaleToUse = chromaticScaleSharps;
        }
        int octaveTraversal = interval / 12;
        auto startIndex = find(scaleToUse.begin(), scaleToUse.end(), note.noteName);
        int index = INT_MAX;
        if (startIndex != scaleToUse.end())
        {
            index = distance(scaleToUse.begin(), startIndex); // ✅ convert iterator to int
        }
        transposedNote.noteName = scaleToUse[(index + interval) % 12];
        transposedNote.octave += octaveTraversal;
        return transposedNote;
    }
};
class Fret
{
public:
    int fretNumber;
    Note note;

    Fret(int fretNumber, Note note)
    {
        this->fretNumber = fretNumber;
        this->note = note;
    }

    Fret() : fretNumber(0), note() {}
    string info()
    {
        return "fret number: " + to_string(fretNumber) + "\n note: " + note.info() + "\n";
    }
};
using ScaleMap = map<string, vector<Fret>>;
class InstrumentString
{
public:
    vector<Fret> frets;
    string name;
    InstrumentString(Note note, int numberOfFrets)
    {
        this->name = note.noteName;
        for (int i = 0; i <= numberOfFrets; i++)
        {
            Note transposedNote = MusicCalculator::transpose(note, i, true);
            Fret transposedFret = {i, transposedNote};
            frets.push_back(transposedFret);
        }
    }
    string info()
    {
        string output = name + " string: ";
        for (Fret fret : frets)
        {
            output += fret.note.info() + ", ";
        }
        output += "\n";
        return output;
    }
};
class Scale
{
private:
    map<string, vector<int>> intervalPatternMap =
        {
            {"major", {2, 2, 1, 2, 2, 2}},
            {"minor", {2, 1, 2, 2, 1, 2}}};

public:
    vector<Note> scaleTones;
    Scale(Note startingNote, string scaleType)
    {
        scaleTones.push_back(startingNote);
        Note scaleTone = startingNote;
        for (int i = 0; i < intervalPatternMap[scaleType].size(); i++)
        {
            scaleTone = MusicCalculator::transpose(scaleTone, intervalPatternMap[scaleType][i], true);
            scaleTones.push_back(scaleTone);
        }
    }
    string info()
    {
        string output = "";
        for (Note tone : scaleTones)
        {
            output += tone.info() + ", ";
        }
        return output + "\n";
    }
    int getScaleDegree(string noteName)
    {
        for (int i = 0; i < scaleTones.size(); i++)
        {
            if (scaleTones[i].noteName == noteName)
            {
                return i + 1;
            }
        }
    }
};
class Fretboard
{
public:
    vector<InstrumentString> strings;
    Fretboard(vector<Note> openNotes, int numberOfFrets)
    {
        for (Note openNote : openNotes)
        {
            InstrumentString generatedString = InstrumentString(openNote, numberOfFrets);
            strings.push_back(generatedString);
        }
    }

    template <typename T>
    bool is_member(const std::vector<T> &vec, const T &element)
    {
        return std::find(vec.begin(), vec.end(), element) != vec.end();
    }

    ScaleMap scaleMap(Scale scale)
    {
        ScaleMap output;
        vector<string> scaleAsStringVector;
        for (Note note : scale.scaleTones)
        {
            scaleAsStringVector.push_back(note.noteName);
        }

        for (InstrumentString instrumentString : strings)
        {
            for (Fret fret : instrumentString.frets)
            {
                if (is_member(scaleAsStringVector, fret.note.noteName))
                {
                    output[instrumentString.frets[0].note.noteName].push_back(fret);
                }
            }
        }
        return output;
    }
    string showScaleMap(ScaleMap scaleMap)
    {
        string output = "";

        for (const auto &[key, value] : scaleMap)
        {
            output += "\n" + key + " string:";
            for (Fret fret : scaleMap[key])
            {
                output += fret.note.info() + " @fret " + to_string(fret.fretNumber) + ", ";
            }
        }
        return output + "\n";
    }
    string info()
    {
        string output = "";
        for (InstrumentString string : strings)
        {
            output += string.info() + "\n";
        }
        return output;
    }
};

int main()
{
    vector<Note> guitarStrings = {{"E", 2}, {"A", 2}, {"D", 3}, {"G", 3}, {"B", 3}, {"E", 4}};
    Fretboard GuitarFretboard = Fretboard(guitarStrings, 24);
    // cout << GuitarFretboard.info() << endl;
    // cout << GuitarFretboard.strings[0].frets[3].note.noteName + "\n";
    auto cMinorScale = Scale(Note("C", 4), "minor");

    ScaleMap cMinorScaleMap = GuitarFretboard.scaleMap(cMinorScale);
    cout << GuitarFretboard.showScaleMap(cMinorScaleMap);
    return 0;
}
