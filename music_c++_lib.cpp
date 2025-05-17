#include <iostream>
#include "mylib.hpp"
#include <map>
#include <algorithm>
#include <cassert>
using namespace std;

class Note
{
public:
    string noteName;
    int octave;
    int scaleDegree = 0;
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
    static const inline vector<string> chromaticScaleSharps = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
    static const inline vector<string> chromaticScaleFlats = {"A", "Bb", "B", "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab"};
    static Note transpose(Note note, int interval, bool accidentalType)
    {
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
        if (index + interval > -1)
        {
            transposedNote.noteName = scaleToUse[(index + interval) % 12];
        }
        else
        {
            // cout << index << " " << interval << (index + interval) % 12;
            if (((index + interval) % 12) == 0)
            {
                transposedNote.noteName = scaleToUse[0];
            }
            else
            {
                transposedNote.noteName = scaleToUse[12 + ((index + interval) % 12)];
            }
        }
        int indexOfC = -1;
        int indexOfTransposedNote = -1;
        int noteIndex = -1;
        for (int i = 0; i < scaleToUse.size(); i++)
        {
            if (scaleToUse[i] == "C")
                indexOfC = i;
            if (scaleToUse[i] == note.noteName)
                noteIndex = i;
            if (scaleToUse[i] == transposedNote.noteName)
                indexOfTransposedNote = i;
        }

        // Handle octave bump across C boundary
        if (indexOfC > -1 && noteIndex > -1 && indexOfTransposedNote > -1)
        {
            bool crossesC = (noteIndex < indexOfC && indexOfTransposedNote >= indexOfC && interval > -1);
            bool wrapsAround = (noteIndex + interval >= static_cast<int>(scaleToUse.size()) && indexOfTransposedNote >= indexOfC && interval > -1);
            bool crossesCNegative = (noteIndex >= indexOfC && indexOfTransposedNote < indexOfC && interval < 0);
            bool wrapsAroundNegative = (noteIndex + interval <= 0 && indexOfTransposedNote < indexOfC && interval < 0);

            // cout << crossesC << wrapsAround << crossesCNegative << wrapsAroundNegative;
            if (crossesC || wrapsAround)
            {
                octaveTraversal += 1;
            }
            if ((crossesCNegative || wrapsAroundNegative) && ((index + interval) % 12) != 0)
            {
                octaveTraversal -= 1;
            }
        }
        transposedNote.octave += octaveTraversal;
        return transposedNote;
    }
    static int getInterval(Note note1, Note note2)
    {
        bool isSharp = true;
        if (note1.noteName.size() > 1 && note1.noteName[1] == 'b')
        {
            isSharp = false;
        }
        if (note1.noteName.size() > 1 && note2.noteName.size() > 1)
        {
            if (note2.noteName[1] != note1.noteName[1])
            {
                note2 = enharmonicEquivalent(note2);
                bool isSharp = note2.noteName[1] == '#';
            }
        }
        int stepDirection = -1;
        if (note2.octave > note1.octave)
        {
            stepDirection = 1;
        }

        if (note2.octave == note1.octave)
        {
            int index1 = -1;
            int index2 = -1;
            auto scaleToUse = isSharp ? chromaticScaleSharps : chromaticScaleFlats;
            for (int i = 0; i < 12; i++)
            {
                if (scaleToUse[i] == note1.noteName)
                    index1 = i;
                if (scaleToUse[i] == note2.noteName)
                    index2 = i;
            }
            if (index1 < index2)
            {
                stepDirection = 1;
            }
        }
        int steps = 0;
        while (note1.info() != note2.info() /* && steps > -15 && steps < 15 */)
        {

            note1 = transpose(note1, stepDirection, !isSharp);
            steps += stepDirection;
            cout << "note1: " + note1.info() + " note 2: " + note2.info() + "\n";
        }
        return steps;
    }
    static Note enharmonicEquivalent(Note note)
    {
        if (note.noteName.size() == 1)
            return note;
        bool isSharp = true;
        if (note.noteName[1] == 'b')
            isSharp = false;
        vector<string> scaleToUse = isSharp ? chromaticScaleSharps : chromaticScaleFlats;
        int indexOfNoteInChromaticScale = -1;
        for (int i = 0; i < 12; i++)
        {
            if (scaleToUse[i] == note.noteName)
            {
                indexOfNoteInChromaticScale = i;
            }
        }
        Note output;
        output.noteName = isSharp ? chromaticScaleFlats[indexOfNoteInChromaticScale] : chromaticScaleSharps[indexOfNoteInChromaticScale];
        output.octave = note.octave;
        output.scaleDegree = note.scaleDegree;
        return output;
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
            Fret transposedFret = Fret(i, transposedNote);
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
    string type;
    Scale(Note startingNote, string scaleType)
    {
        type = scaleType;
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
        return -1;
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
                    fret.note.scaleDegree = scale.getScaleDegree(fret.note.noteName);
                    output[instrumentString.frets[0].note.info()].push_back(fret);
                }
            }
        }
        return output;
    }
    string showScaleMap(ScaleMap scaleMap)
    {
        string output = "";

        for (InstrumentString instrumentString : strings)
        {
            output += "\n" + instrumentString.frets[0].note.info() + " string: ";
            for (Fret fret : scaleMap[instrumentString.frets[0].note.info()])
            {
                output += fret.note.info() + "(" + to_string(fret.note.scaleDegree) + ")" + " @fret " + to_string(fret.fretNumber) + ", ";
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
class Chord
{
private:
    map<string, vector<int>> quality =
        {{"major", {4, 7}},
         {"minor", {3, 7}}};

public:
    vector<Note> notes;
    Chord(Note note, string chordType)
    {
        bool isFlat = true;
        if (note.noteName.size() > 1 && note.noteName[1] == '#')
        {
            isFlat = false;
        }
        notes.push_back(note);
        for (int interval : quality[chordType])
        {
            notes.push_back(MusicCalculator::transpose(note, interval, isFlat));
        }
    }
    Chord(Note note, string chordType, int inversionType)
    {
        bool isFlat = true;
        if (note.noteName.size() > 1 && note.noteName[1] == '#')
        {
            isFlat = false;
        }
        notes.push_back(note);
        for (int interval : quality[chordType])
        {
            notes.push_back(MusicCalculator::transpose(note, interval, isFlat));
        }
        Note root = notes[0];
        Note third = notes[1];
        Note fifth = notes[2];
        if (inversionType == 1)
        {
            third.octave -= 1;
            fifth.octave -= 1;
            notes = {third, fifth, root};
        }
        if (inversionType == 2)
        {
            fifth.octave -= 1;
            notes = {fifth, root, third};
        }
    }
    Chord(vector<Note> notes)
    {
        this->notes = notes;
    }
    string info()
    {
        string output = "";
        for (Note note : notes)
        {
            output += note.info() + ", ";
        }
        return output + "\n";
    }
};
class ChordProgression
{
public:
    vector<Chord> chords;
    ChordProgression(Scale scale, vector<int> chordNumber)
    {
        for (int chord : chordNumber)
        {
            Note root = scale.scaleTones[(chord - 1) % scale.scaleTones.size()];
            Note third = scale.scaleTones[(chord + 1) % scale.scaleTones.size()];
            Note fifth = scale.scaleTones[(chord + 3) % scale.scaleTones.size()];
            chords.push_back(Chord({root, third, fifth}));
        }
    }
    string info()
    {
        string output = "";
        for (Chord chord : chords)
        {
            output += chord.info();
        }
        return output += "\n";
    }
};

int main()
{
    // vector<Note> guitarStrings = {{"E", 2}, {"A", 2}, {"D", 3}, {"G", 3}, {"B", 3}, {"E", 4}};
    // Fretboard GuitarFretboard = Fretboard(guitarStrings, 24);
    // cout << GuitarFretboard.info() << endl;
    // cout << GuitarFretboard.strings[0].frets[3].note.noteName + "\n";
    auto cMinorScale = Scale(Note("C", 4), "minor");
    // ScaleMap cMinorScaleMap = GuitarFretboard.scaleMap(cMinorScale);
    // cout << GuitarFretboard.showScaleMap(cMinorScaleMap);
    // InstrumentString lowE = InstrumentString(Note("E", 2), 12);
    // cout << lowE.info();
    // cout << MusicCalculator::transpose(Note("D", 3), -1, false).info() + "\n";
    // Note a = Note("A#", 4);
    // a = MusicCalculator::enharmonicEquivalent(a);
    // cout << a.info();
    // Chord cMinorChord = Chord(Note("C", 4), "major");
    // cout << cMinorChord.info() << endl;
    cout << ChordProgression(cMinorScale, {1, 4, 5}).info();
    return 0;
}
