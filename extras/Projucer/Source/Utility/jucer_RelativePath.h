/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#pragma once


//==============================================================================
/** Manipulates a cross-platform partial file path. (Needed because File is designed
    for absolute paths on the active OS)
*/
class RelativePath
{
public:
    //==============================================================================
    enum RootFolder
    {
        unknown,
        projectFolder,
        buildTargetFolder
    };

    //==============================================================================
    RelativePath()
        : root (unknown)
    {}

    RelativePath (const String& relPath, const RootFolder rootType)
        : path (FileHelpers::unixStylePath (relPath)), root (rootType)
    {
    }

    RelativePath (const File& file, const File& rootFolder, const RootFolder rootType)
        : path (FileHelpers::unixStylePath (FileHelpers::getRelativePathFrom (file, rootFolder))), root (rootType)
    {
    }

    RootFolder getRoot() const                              { return root; }

    String toUnixStyle() const                              { return FileHelpers::unixStylePath (path); }
    String toWindowsStyle() const                           { return FileHelpers::windowsStylePath (path); }

    String getFileName() const                              { return getFakeFile().getFileName(); }
    String getFileNameWithoutExtension() const              { return getFakeFile().getFileNameWithoutExtension(); }

    String getFileExtension() const                         { return getFakeFile().getFileExtension(); }
    bool hasFileExtension (juce::StringRef extension) const { return getFakeFile().hasFileExtension (extension); }
    bool isAbsolute() const                                 { return FileHelpers::isAbsolutePath (path); }

    RelativePath withFileExtension (const String& extension) const
    {
        return RelativePath (path.upToLastOccurrenceOf (".", ! extension.startsWithChar ('.'), false) + extension, root);
    }

    RelativePath getParentDirectory() const
    {
        String p (path);
        if (path.endsWithChar ('/'))
            p = p.dropLastCharacters (1);

        return RelativePath (p.upToLastOccurrenceOf ("/", false, false), root);
    }

    RelativePath getChildFile (const String& subpath) const
    {
        if (FileHelpers::isAbsolutePath (subpath))
            return RelativePath (subpath, root);

        String p (toUnixStyle());
        if (! p.endsWithChar ('/'))
            p << '/';

        return RelativePath (p + subpath, root);
    }

    RelativePath rebased (const File& originalRoot, const File& newRoot, const RootFolder newRootType) const
    {
        if (isAbsolute())
            return RelativePath (path, newRootType);

        return RelativePath (FileHelpers::getRelativePathFrom (originalRoot.getChildFile (toUnixStyle()), newRoot), newRootType);
    }

private:
    //==============================================================================
    String path;
    RootFolder root;

    File getFakeFile() const
    {
        // This method gets called very often, so we'll cache this directory.
        static const File currentWorkingDirectory (File::getCurrentWorkingDirectory());
        return currentWorkingDirectory.getChildFile (path);
    }
};
