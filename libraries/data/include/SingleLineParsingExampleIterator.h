////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SingleLineParsingExampleIterator.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AutoDataVector.h"
#include "Example.h"
#include "ExampleIterator.h"

#include <memory>
#include <vector>

namespace ell
{
namespace data
{
    /// <summary>
    /// An Example iterator that parses line by line (where line order is determined by a text line
    /// iterator. For each line, a metadata parser is applied first and a datavector parser is
    /// applied second.
    /// </summary>
    ///
    /// <typeparam name="TextLineIteratorType"> TextLine iterator type. </typeparam>
    /// <typeparam name="MetadataParserType"> Metadata parser type. </typeparam>
    /// <typeparam name="DataVectorParserType"> DataVector parser type. </typeparam>
    template <typename TextLineIteratorType, typename MetadataParserType, typename DataVectorParserType>
    class SingleLineParsingExampleIterator : public IExampleIterator<ParserExample<DataVectorParserType, MetadataParserType>>
    {
    public:
        using ExampleType = ParserExample<DataVectorParserType, MetadataParserType>;

        /// <summary> Constructs a SingleLineParsingExampleIterator. </summary>
        ///
        /// <param name="textLineIterator"> The row iterator. </param>
        /// <param name="metadataParser"> The metadata parser. </param>
        /// <param name="dataVectorParser"> The data vector parser. </param>
        SingleLineParsingExampleIterator(TextLineIteratorType textLineIterator, MetadataParserType metadataParser, DataVectorParserType dataVectorParser);

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if the iterator is valid, false otherwise. </returns>
        bool IsValid() const override { return _textLineIterator.IsValid(); }

        /// <summary> Proceeds to the next example. </summary>
        void Next() override;

        /// <summary> Gets the current example. </summary>
        ///
        /// <returns> A SupervisedExample. </returns>
        ExampleType Get() const override { return _currentExample; };

    private:
        void ReadExample();

        TextLineIteratorType _textLineIterator;
        MetadataParserType _metadataParser;
        DataVectorParserType _dataVectorParser;
        ExampleType _currentExample;
    };

    /// <summary>
    /// Helper function that creates a SingleLineParsingExampleIterator from a line iterator, a
    /// metadata parser, and a datavector parser.
    /// </summary>
    ///
    /// <typeparam name="TextLineIteratorType"> Text line iterator type. </typeparam>
    /// <typeparam name="MetadataParserType"> Metadata parser type. </typeparam>
    /// <typeparam name="DataVectorParserType"> Data vector parser type. </typeparam>
    /// <param name="textLineIterator"> The line iterator. </param>
    /// <param name="metadataParser"> The metadata parser. </param>
    /// <param name="dataVectorParser"> The data vector parser. </param>
    ///
    /// <returns> The single-line parsing example iterator. </returns>
    template <typename TextLineIteratorType, typename MetadataParserType, typename DataVectorParserType>
    auto MakeSingleLineParsingExampleIterator(TextLineIteratorType textLineIterator, MetadataParserType metadataParser, DataVectorParserType dataVectorParser);
} // namespace data
} // namespace ell

#pragma region implementation

namespace ell
{
namespace data
{
    template <typename TextLineIteratorType, typename MetadataParserType, typename DataVectorParserType>
    SingleLineParsingExampleIterator<TextLineIteratorType, MetadataParserType, DataVectorParserType>::SingleLineParsingExampleIterator(TextLineIteratorType textLineIterator, MetadataParserType metadataParser, DataVectorParserType dataVectorParser) :
        _textLineIterator(std::move(textLineIterator)),
        _metadataParser(std::move(metadataParser)),
        _dataVectorParser(std::move(dataVectorParser))
    {
        if (_textLineIterator.IsValid())
        {
            ReadExample();
        }
    }

    template <typename TextLineIteratorType, typename MetadataParserType, typename DataVectorParserType>
    void SingleLineParsingExampleIterator<TextLineIteratorType, MetadataParserType, DataVectorParserType>::Next()
    {
        _textLineIterator.Next();
        ReadExample();
    }

    template <typename TextLineIteratorType, typename MetadataParserType, typename DataVectorParserType>
    void SingleLineParsingExampleIterator<TextLineIteratorType, MetadataParserType, DataVectorParserType>::ReadExample()
    {
        // get a line - skip lines that contain just whitespace or just a comment
        TextLine line = _textLineIterator.GetTextLine();
        line.TrimLeadingWhitespace();

        while (line.IsEndOfContent())
        {
            _textLineIterator.Next();
            if (!_textLineIterator.IsValid())
            {
                return;
            }

            line = _textLineIterator.GetTextLine();
            line.TrimLeadingWhitespace();
        }

        // parse metadata
        auto metaData = _metadataParser.Parse(line);

        // parse datavector
        auto dataVector = _dataVectorParser.Parse(line);

        // cache the parsed example
        _currentExample = ExampleType(std::move(dataVector), std::move(metaData));
    }

    template <typename TextLineIteratorType, typename MetadataParserType, typename DataVectorParserType>
    auto MakeSingleLineParsingExampleIterator(TextLineIteratorType textLineIterator, MetadataParserType metadataParser, DataVectorParserType dataVectorParser)
    {
        using ExampleType = ParserExample<DataVectorParserType, MetadataParserType>;
        using IteratorType = SingleLineParsingExampleIterator<TextLineIteratorType, MetadataParserType, DataVectorParserType>;
        auto iterator = std::make_unique<IteratorType>(std::move(textLineIterator), std::move(metadataParser), std::move(dataVectorParser));
        return ExampleIterator<ExampleType>(std::move(iterator));
    }
} // namespace data
} // namespace ell

#pragma endregion implementation
