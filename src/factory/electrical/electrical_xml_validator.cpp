// File: electrical_xml_validator.cpp
// Author: Christopher Abele

#include "electrical_xml_validator.h"

#include "../../exceptions/error_proto.h"

#include <cmath>


namespace validator
{

// Helpfunctions
void CheckForChildrenOrException( const boost::shared_ptr< xmlparser::XmlParameter >& node, const char* parentNodeName )
{
    if ( node->GetNumberOfChildren() == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoChildrenException",
                                             node->GetElementName(), parentNodeName );
}

void CheckForChildrenOrException( const std::initializer_list< boost::shared_ptr< xmlparser::XmlParameter > >& nodeList,
                                  const char* parentNodeName )
{
    for ( const auto& node : nodeList )
        CheckForChildrenOrException( node, parentNodeName );
}

void CheckForAttributeOrException( const boost::shared_ptr< xmlparser::XmlParameter >& node, const char* attributeName,
                                   const char* attributeValue, const char* parentNodeName )
{
    if ( !node->HasElementAttribute( attributeName, attributeValue ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AttributeRequiredException",
                                             node->GetElementName(), parentNodeName, attributeName, attributeValue );
}

void CheckForChildAttributeOrException( const boost::shared_ptr< xmlparser::XmlParameter >& node, const char* attributeName,
                                        const char* attributeValue, const char* parentNodeName, const bool positiveCheck )
{

    if ( positiveCheck )
    {
        if ( !node->HasElementAnyChildWithAttribute( attributeName, attributeValue ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ChildWithAttributeRequiredException",
                                                 node->GetElementName(), parentNodeName, attributeName, attributeValue );
    }

    else
    {
        if ( node->HasElementAnyChildWithAttribute( attributeName, attributeValue ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "ChildWithAttributeNotAllowedException", node->GetElementName(),
                                                 parentNodeName, attributeName, attributeValue );
    }
}

void CheckForChildAttributeOrException( const std::initializer_list< boost::shared_ptr< xmlparser::XmlParameter > >& nodeList,
                                        const char* attributeName, const char* attributeValue,
                                        const char* parentNodeName, const bool positiveCheck )
{
    for ( const auto& node : nodeList )
        CheckForChildAttributeOrException( node, attributeName, attributeValue, parentNodeName, positiveCheck );
}

void CheckForChildAttributeOrException( const boost::shared_ptr< xmlparser::XmlParameter >& node, const char* attributeName,
                                        const std::initializer_list< const char* >& attributeValueList,
                                        const char* parentNodeName, const bool positiveCheck )
{
    for ( const char* attributeValue : attributeValueList )
        CheckForChildAttributeOrException( node, attributeName, attributeValue, parentNodeName, positiveCheck );
}

void CheckForEqualVectorSizeOrException( const std::initializer_list< std::vector< double > >& vectorList, const size_t size,
                                         const std::initializer_list< const char* >& nodeNames, const char* parentNodeName )
{
    std::string nodeNamesOutput = "";

    // Format output for exception
    for ( const auto& nodeName : nodeNames )
    {
        nodeNamesOutput += std::string( nodeName ) + std::string( "/" );
    }

    // Checks the vector sizes
    for ( const auto& vector : vectorList )
    {
        const size_t sizeOfActualVector = vector.size();

        if ( sizeOfActualVector != size )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "VectorContentHasNotSameLengthException", nodeNamesOutput.c_str(), parentNodeName );
    }
}


// Validation functions
void ValidateCellelementNode( const xmlparser::XmlParameter* cellelementNode, const size_t cellCounter, const bool aging )
{
    const char* nameOfCellelementNode = cellelementNode->GetElementName();

    const bool hasAnodeNode = cellelementNode->HasElementDirectChild( "Anode" );
    const bool hasCathodeNode = cellelementNode->HasElementDirectChild( "Cathode" );
    const bool hasOtherNode = cellelementNode->HasElementDirectChild( "Other" );

    const bool hasChildrenNode = cellelementNode->HasElementDirectChild( "Children" );

    bool isHalfcellSimulation = ( hasAnodeNode && hasCathodeNode && hasOtherNode && !hasChildrenNode );
    bool isFullcellSimulation = ( !hasAnodeNode && !hasCathodeNode && !hasOtherNode && hasChildrenNode );

    // Halfcellsimulation
    if ( isHalfcellSimulation )
    {
        const boost::shared_ptr< xmlparser::XmlParameter > anodeNode = cellelementNode->GetElementChild( "Anode" );
        const boost::shared_ptr< xmlparser::XmlParameter > cathodeNode = cellelementNode->GetElementChild( "Cathode" );

        const std::vector< boost::shared_ptr< xmlparser::XmlParameter > > anodeChildren =
         anodeNode->GetElementChildren( "Children" );
        const std::vector< boost::shared_ptr< xmlparser::XmlParameter > > cathodeChildren =
         cathodeNode->GetElementChildren( "Children" );

        const boost::shared_ptr< xmlparser::XmlParameter > anodeChildrenNode = anodeNode->GetElementChild( "Children" );
        const boost::shared_ptr< xmlparser::XmlParameter > cathodeChildrenNode =
         cathodeNode->GetElementChild( "Children" );

        // AnodeChildrenNode and CathodeChildrenNode needs to have children
        CheckForChildrenOrException( {anodeChildrenNode, cathodeChildrenNode}, nameOfCellelementNode );

        // AnodeChildrenNode may not have a child with an other class than AnodeElement
        for ( const auto& anodeChild : anodeChildren )
            CheckForAttributeOrException( anodeChild, "class", "AnodeElement", nameOfCellelementNode );

        // CathodeChildrenNode may not have a child with an other class than CathodeElement
        for ( const auto& cathodeChild : cathodeChildren )
            CheckForAttributeOrException( cathodeChild, "class", "CathodeElement", nameOfCellelementNode );
    }

    // Fullcellsimulation
    else if ( isFullcellSimulation )
    {
        ValidateTwoPortWithChild( cellelementNode );

        const boost::shared_ptr< xmlparser::XmlParameter > childrenNode =
         cellelementNode->GetElementChild( "Children" );

        // CellelementNode needs to have children
        CheckForChildrenOrException( childrenNode, nameOfCellelementNode );

        // CellelementNode may not have a Anode- or CathodeElement
        CheckForChildAttributeOrException( childrenNode, "class", {"AnodeElement", "CathodeElement"}, nameOfCellelementNode, false );

        // CellelementNode needs to own a VoltageSource, Capacity or ParallelRC
        /*
        if ( !cellelementNode->HasElementAnyChildWithAttribute( "class", "VoltageSource" ) &&
             !cellelementNode->HasElementAnyChildWithAttribute( "class", "Capacity" ) &&
             !cellelementNode->HasElementAnyChildWithAttribute( "class", "ParallelRC" ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "ChildWithAttributeRequiredException",
                                                 cellelementNode->GetElementName(), nameOfCellelementNode, "class",
                                                 "VoltageSource, Capacity or ParallelRC" );
                 */
    }

    // Only halfcell or fullsimulation allowed
    else
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                             "NotConfiguredForHalfcellOrFullcellSimulationException", nameOfCellelementNode );

#ifdef BUILD_AGING_SIMULATION

    if ( aging )
    {
        const auto agingNodeGlobal = cellelementNode->GetConfigurationRoot()->GetElementChild( "Aging" );

        // Collects the capacity and resistance factors for total aging
        const std::vector< double > capacityFactors =
         agingNodeGlobal->GetElementDoubleVecValue( "InitialCapacityFactor" );
        const std::vector< double > resistanceFactors =
         agingNodeGlobal->GetElementDoubleVecValue( "InitialResistanceFactor" );

        const auto calendarianAgingNodeGlobal = agingNodeGlobal->GetElementChild( "Calendarian" );

        // Collects the capacity and resistance factors and cell age values for calendarian aging
        const std::vector< double > calendarianCapacityFactors =
         calendarianAgingNodeGlobal->GetElementDoubleVecValue( "InitialCapacityFactor" );
        const std::vector< double > calendarianResistanceFactors =
         calendarianAgingNodeGlobal->GetElementDoubleVecValue( "InitialResistanceFactor" );
        const std::vector< double > cellAgeValues =
         calendarianAgingNodeGlobal->GetElementDoubleVecValue( "InitialCellAge" );

        const auto cyclicalAgingNodeGlobal = agingNodeGlobal->GetElementChild( "Cyclical" );

        // Collects the capacity and resistance factors and cell age values for cylical aging
        const std::vector< double > cyclicalCapacityFactors =
         cyclicalAgingNodeGlobal->GetElementDoubleVecValue( "InitialCapacityFactor" );
        const std::vector< double > cyclicalResistanceFactors =
         cyclicalAgingNodeGlobal->GetElementDoubleVecValue( "InitialResistanceFactor" );
        const std::vector< double > ChargeThroughputValues =
         cyclicalAgingNodeGlobal->GetElementDoubleVecValue( "InitialChargeThroughput" );


        // The size of the vector "capacityFactors" is the reference size for the other vectors
        const size_t size = capacityFactors.size();

        // Checks if all aging vectors have the same length
        CheckForEqualVectorSizeOrException(
         {capacityFactors, resistanceFactors, calendarianCapacityFactors, calendarianResistanceFactors, cellAgeValues,
          cyclicalCapacityFactors, cyclicalResistanceFactors, ChargeThroughputValues},
         size, {"InitialCapacityFactor", "InitialResistanceFactor", "InitialCellAge", "InitialChargeThroughput"},
         "Aging" );

        // If size of the vectors < number of cells
        if ( size < cellCounter + 1 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "WrongRelationException", "Aging",
                                                 "Number size of aging factors >= number of cells" );

        // Checks if totalCapacityFactor = 1 - calendarianCapacity - cyclicalCapacityFactor
        for ( size_t i = 0; i < size; ++i )
        {
            if ( std::abs( 1.0 - calendarianCapacityFactors[i] - cyclicalCapacityFactors[i] - capacityFactors[i] ) > 0.00001 )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "WrongRelationException", "Aging",
                                                     "InitialCapacityFactor = 1 - "
                                                     "Calendarian->InitialCapacityFactor[i] - "
                                                     "Cycical->InitialCapacityFactor[i]" );

            if ( std::abs( 1.0 + calendarianResistanceFactors[i] + cyclicalResistanceFactors[i] - resistanceFactors[i] ) > 0.00001 )
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "WrongRelationException", "Aging",
                                                     "InitialResistanceFactor = 1 + "
                                                     "Calendarian->InitialResistanceFactor[i] + "
                                                     "Cycical->InitialResistanceFactor[i]" );
        }
    }

#endif
}

void ValidateEletrodeElementNode( const xmlparser::XmlParameter* electrodeElementNode )
{
    ValidateTwoPortWithChild( electrodeElementNode );

    const boost::shared_ptr< xmlparser::XmlParameter > childrenNode =
     electrodeElementNode->GetElementChild( "Children" );

    const char* nameOfElectrodeElementNode = electrodeElementNode->GetElementName();

    // ElectrodeElementNode can only have pure electrical elements as child
    CheckForChildAttributeOrException( childrenNode, "class", {"SerialTwoPort", "ParallelTwoPort", "ChainedTwoPort"},
                                       nameOfElectrodeElementNode, false );

    // ElectrodeElementNode needs a VoltageSource element as child
    CheckForChildAttributeOrException( childrenNode, "class", "VoltageSource", nameOfElectrodeElementNode );
}

void ValidateAnodeElementNode( const xmlparser::XmlParameter* anodeElementNode, const size_t anodeElementCounter )
{
    ValidateEletrodeElementNode( anodeElementNode );

    const boost::shared_ptr< xmlparser::XmlParameter > anodeNode = anodeElementNode->GetElementParent()->GetElementParent();

    if ( anodeNode->HasElementDirectChild( "Variation" ) )
    {
        const boost::shared_ptr< xmlparser::XmlParameter > variationNode = anodeNode->GetElementChild( "Variation" );

        const std::vector< double > resistanceVariationValues = variationNode->GetElementDoubleVecValue( "Resistance" );

        if ( resistanceVariationValues.size() < anodeElementCounter + 1 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "WrongRelationException", "Variation",
                                                 "Number size of resistance variation factors >= number of "
                                                 "AnodeElements" );
    }
}

void ValidateTwoPortWithChild( const xmlparser::XmlParameter* twoPortWithChildNode )
{
    const boost::shared_ptr< xmlparser::XmlParameter > childrenNode =
     twoPortWithChildNode->GetElementChild( "Children" );

    const char* nameOfTwoPortWithChildNode = twoPortWithChildNode->GetElementName();

    // TwoPortWithChild needs to have children
    CheckForChildrenOrException( childrenNode, nameOfTwoPortWithChildNode );
}

void ValidateSerialTwoPortNode( const xmlparser::XmlParameter* serialTwoPortNode )
{
    ValidateTwoPortWithChild( serialTwoPortNode );
}

void ValidateParallelTwoPortNode( const xmlparser::XmlParameter* parallelTwoPortNode )
{
    ValidateTwoPortWithChild( parallelTwoPortNode );
}

}    // End namespace validator
