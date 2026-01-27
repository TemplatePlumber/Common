#pragma once

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

    template<typename NODE_LIST_T,
            typename ADJACENCY_FNC_T, 
            typename HEURISTIC_FNC_T>
    class DynamicAStar {
    public:
        using TNode = typename NODE_LIST_T::value_type;
        using TDistance = std::invoke_result_t<HEURISTIC_FNC_T,TNode,TNode>;
        HEURISTIC_FNC_T _distanceFnc;
        DynamicAStar(){}
        DynamicAStar(const NODE_LIST_T & nodes, ADJACENCY_FNC_T adjacencyFnc, HEURISTIC_FNC_T heuristicFnc) : _distanceFnc(heuristicFnc)
        {
            initialize(nodes,adjacencyFnc,heuristicFnc);
        }
        
        void initialize(const NODE_LIST_T & nodes, ADJACENCY_FNC_T adjacencyFnc, HEURISTIC_FNC_T heuristicFnc)
        {
            _distanceFnc = heuristicFnc;
            std::size_t id=0;
            for(const auto & node : nodes)
            {
                _nodesById[id] = node;
                id++;
            }

            for(const auto & [a,node1] : _nodesById)
            {
                auto & topologyByFirstNode = _topology[a];
                for(const auto & [b,node2] : _nodesById)
                {
                    CONTINUE_IF(a == b);
                    
                    if(adjacencyFnc(node1, node2))
                    {
                        topologyByFirstNode += b;
                    }
                }
            }
            
            ASTAR_DEBUG(
                std::cout << "Initialized AStar Network" << std::endl;
                std::cout << Tp::toJson(_topology) << std::endl;
                if constexpr(requires { Tp::toJson(_nodesById); })
                {
                    std::cout << Tp::toJson(_nodesById) << std::endl;
                }
            )
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
        
        std::vector<TNode> findPath(const TNode & bgnNode,const TNode & endNode) const
        {
            try
            {
                std::set<std::uint64_t> expanded;
                std::set<_SearchNode> openList;
                
                const auto bgnId = _getNodeIndex(bgnNode);
                const auto endId = _getNodeIndex(endNode);     
                auto node = 
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

                        auto dist_C_N = _distanceFnc(curNode,nxtNode);
                        auto dist_N_E = _distanceFnc(nxtNode,endNode);
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
        
        std::unordered_map<std::uint64_t,TNode> _nodesById;
        std::unordered_map<std::uint64_t,std::vector<std::uint64_t>> _topology;
    };





}