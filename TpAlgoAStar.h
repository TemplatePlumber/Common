#pragma once

#include <functional>

#define ENABLE_ASTAR_DEBUG 0
#if ENABLE_ASTAR_DEBUG
#define ASTAR_DEBUG(...) __VA_ARGS__
#else
#define ASTAR_DEBUG(...)
#endif

namespace Tp
{


//    ASTAR_DEBUG(
//        struct AStarTestVector2 { double x=0.0; double y=0.0; };
//        inline void astarDebugFunction()
//        {
//            std::vector<AStarTestVector2> astar_dbg_points = {
//                {0,0}, {1,0}, {2,0},
//                              {2,1},
//                {0,2}, {1,2}, {2,2}, {3,2}, {4,2},
//                              {2,3},        {4,3},
//                              {2,4}, {3,4}, {4,4}
//            };
//
//            auto LAdjacency = [](AStarTestVector2 node1, AStarTestVector2 node2){
//                std::cout << Tp::areEqual(node1.distance_to(node2),1.0) << std::endl;
//                return Tp::areEqual(node1.distance_to(node2),1.0);
//            };
//
//            auto LHeuristic = [](AStarTestVector2 node1, AStarTestVector2 node2){ 
//                return node1.distance_to(node2);
//            };
//
//            DynamicAStar astar(points,LAdjacency,LHeuristic);
//                auto rslt = astar.findPath({1,0},{4,4});
//                std::cout << Tp::toJson(rslt) << std::endl;
//        }
//    )

    template<typename TNode>
    class DynamicAStar {
    public:
        using TDistance = double;
        
        using ADJACENCY_FNC_T = std::function<bool(TNode,TNode)>;
        using COST_FNC_T      = std::function<double(TNode,TNode)>;
        using HEURISTIC_FNC_T = std::function<double(TNode,TNode)>;
        
        ADJACENCY_FNC_T _adjacencyFnc;
        COST_FNC_T _costFnc;
        HEURISTIC_FNC_T _heuristicFnc;

        std::unordered_map<std::uint64_t,TNode> _nodesById;
        std::unordered_map<std::uint64_t,std::vector<std::uint64_t>> _topology;
        
        DynamicAStar(){}
        
        template<typename NODE_LIST_T>
        void initialize(
            const NODE_LIST_T & nodes,
            ADJACENCY_FNC_T adjacencyFnc,
            COST_FNC_T costFnc, 
            HEURISTIC_FNC_T heuristicFnc)
        {
            _heuristicFnc = heuristicFnc;
            _adjacencyFnc = adjacencyFnc;
            _costFnc = costFnc;
            std::size_t id=0;
            for(const auto & node : nodes)
            {
                _nodesById[id] = node;
                id++;
            }
            
            updateAdjacencies();

            
            ASTAR_DEBUG(
                std::cout << "Initialized AStar Network" << std::endl;
                std::cout << Tp::toJson(_topology) << std::endl;
                if constexpr(requires { Tp::toJson(_nodesById); })
                {
                    std::cout << Tp::toJson(_nodesById) << std::endl;
                }
            )
        }
        
        
        void updateAdjacencies()
        {
            for(const auto & [a,node1] : _nodesById)
            {
                auto & topologyByFirstNode = _topology[a];
                for(const auto & [b,node2] : _nodesById)
                {
                    CONTINUE_IF(a == b);
                    
                    const bool found = Tp::overlap(topologyByFirstNode,b);
                    const bool adjacent = _adjacencyFnc(node1, node2);
                    if(!found && adjacent)
                    {
                        topologyByFirstNode += b;
                    }
                    else if(found && !adjacent)
                    {
                        topologyByFirstNode -= b;
                    }
                }
            }
        }
        
        
        struct _SearchNode
        {
            std::uint64_t endPoint;
            std::vector<std::uint64_t> path;
            TDistance f = {};
            TDistance g = {};
            TDistance h = {};
            
            bool operator<(const _SearchNode & other) const
            {
                return f < other.f;
            }
        };
        
        template<typename AGENT_T,typename EDGE_CULL_FNC_T>
        std::vector<TNode> findAgenticPath(
            const AGENT_T & agent,
            const TNode & bgnNode,
            const TNode & endNode,
            EDGE_CULL_FNC_T edgeCullFunction) const
        {
            try
            {
                std::set<std::uint64_t> expanded;
                std::set<_SearchNode> openList;
                
                const auto bgnId = _getNodeIndex(bgnNode);
                const auto endId = _getNodeIndex(endNode);     
                openList.insert(
                    _SearchNode{
                        .endPoint=bgnId,
                        .path={},
                        .f={},
                        .g={},
                        .h={}
                    });

                while (!openList.empty())
                {
                    auto currSN = *openList.begin();
                    openList.erase(openList.begin());

                    expanded += currSN.endPoint;
                    
                    auto curId = currSN.endPoint;
                    ASTAR_DEBUG(std::cout << "Expanding (" << curId << ")" << std::endl);
                    auto curNode = _getNodeByIndex(curId);
                    if(curId == endId)
                    {
                        std::reverse(currSN.path.begin(),currSN.path.end());
                        currSN.path += endId;
                        
                        return currSN.path | Tp::Transform([&](auto index){ return _getNodeByIndex(index); });
                    }
                    
                    for(const auto & nxtId : _topology.at(curId))
                    {
                        auto & nxtNode = _getNodeByIndex(nxtId);
                        if(Tp::overlap(expanded,nxtId))
                        {
                            ASTAR_DEBUG(std::cout << "Skip expanded node(" << nxtId << ")" << std::endl);
                            continue;
                        }
                        
                        if(!edgeCullFunction(agent,curNode,nxtNode))
                        {
                            continue;
                        }

                        auto dist_C_N = _costFnc(curNode,nxtNode);
                        auto dist_N_E = _heuristicFnc(nxtNode,endNode);
                        ASTAR_DEBUG(std::cout << "("<< curId << "->" << nxtId << ") g: " << dist_C_N << " h: " << dist_N_E << std::endl);
                        openList.insert(
                            _SearchNode{
                                .endPoint=nxtId,
                                .path=std::vector<std::uint64_t>{} + currSN.endPoint + currSN.path,
                                .f = dist_C_N + dist_N_E,
                                .g = dist_C_N,
                                .h = dist_N_E
                            }
                        );
                    }
                }
            }
            catch(const std::out_of_range &e)
            {
                std::cout << "Caught out of range exception: " << e.what() << std::endl;
            }
            
            return {};
        }
        
        std::vector<TNode> findPath(const TNode & bgnNode,const TNode & endNode) const
        {
            try
            {
                std::set<std::uint64_t> expanded;
                std::set<_SearchNode> openList;
                
                const auto bgnId = _getNodeIndex(bgnNode);
                const auto endId = _getNodeIndex(endNode);     
                openList.insert(
                    _SearchNode{
                        .endPoint=bgnId,
                        .path={},
                        .f={},
                        .g={},
                        .h={}
                    });

                while (!openList.empty())
                {
                    auto currSN = *openList.begin();
                    openList.erase(openList.begin());

                    expanded += currSN.endPoint;
                    
                    auto curId = currSN.endPoint;
                    ASTAR_DEBUG(std::cout << "Expanding (" << curId << ")" << std::endl);
                    auto curNode = _getNodeByIndex(curId);
                    if(curId == endId)
                    {
                        std::reverse(currSN.path.begin(),currSN.path.end());
                        currSN.path += endId;
                        
                        return currSN.path | Tp::Transform([&](auto index){ return _getNodeByIndex(index); });
                    }
                    
                    for(const auto & nxtId : _topology.at(curId))
                    {
                        auto & nxtNode = _getNodeByIndex(nxtId);
                        if(Tp::overlap(expanded,nxtId))
                        {
                            ASTAR_DEBUG(std::cout << "Skip expanded node(" << nxtId << ")" << std::endl);
                            continue;
                        }

                        auto dist_C_N = _costFnc(curNode,nxtNode);
                        auto dist_N_E = _heuristicFnc(nxtNode,endNode);
                        ASTAR_DEBUG(std::cout << "("<< curId << "->" << nxtId << ") g: " << dist_C_N << " h: " << dist_N_E << std::endl);
                        openList.insert(
                            _SearchNode{
                                .endPoint=nxtId,
                                .path=std::vector<std::uint64_t>{} + currSN.endPoint + currSN.path,
                                .f = dist_C_N + dist_N_E,
                                .g = dist_C_N,
                                .h = dist_N_E
                            }
                        );
                    }
                }
            
            }
            catch(const std::out_of_range &e)
            {
                std::cout << "Caught out of range exception: " << e.what() << std::endl;
            }
            
            return {};
        }
        

        std::uint64_t _getNodeIndex(const TNode & node) const
        {
            for(const auto & [id,containerNode] : _nodesById)
            {
                if(containerNode == node)
                {
                    return id;
                }
            }
            throw std::out_of_range("Could not find AStar node");
        }
        
        
        const TNode & _getNodeByIndex(const std::uint64_t index) const
        {
            return _nodesById.at(index);
        }
    };




#if 0
    template<typename TNode>
    struct AppendedNode
    {
        TNode node;
        BMap<u64,TAgent> usageMap;
    };
    
    template<typename TNode,typename TAgent>
    class TemporalAStar : protected DynamicAStar<AppendedNode<TNode>>
    {
        using PARENT_T = DynamicAStar<AppendedNode<TNode>>;
        using INNER_NODE = AppendedNode<TNode>;
        using ADJACENCY_FNC_T = typename PARENT_T::ADJACENCY_FNC_T;
        using COST_FNC_T = typename PARENT_T::COST_FNC_T;
        using HEURISTIC_FNC_T = typename PARENT_T::HEURISTIC_FNC_T;
        
        
        u64 time=0;
        

        
        template<typename NODE_LIST_T>
        void initialize(
            const NODE_LIST_T & nodes,
            ADJACENCY_FNC_T adjacencyFnc,
            COST_FNC_T costFnc, 
            HEURISTIC_FNC_T heuristicFnc)
        {
            _heuristicFnc = heuristicFnc;
            _adjacencyFnc = adjacencyFnc;
            _costFnc = costFnc;
            
            auto LModifiedAdjacency = [](INNER_NODE n1, INNER_NODE n2){
                
            };
        }
    }
#endif



}