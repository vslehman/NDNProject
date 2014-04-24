#include <iostream>
#include <cmath>
#include "lsdb.hpp"
#include "routing-table-calculator.hpp"
#include "map.hpp"
#include "lsa.hpp"
#include "nexthop.hpp"
#include "nlsr.hpp"

namespace nlsr {

using namespace std;

void
RoutingTableCalculator::allocateAdjMatrix()
{
  adjMatrix = new double*[numOfRouter];
  for (int i = 0; i < numOfRouter; ++i)
  {
    adjMatrix[i] = new double[numOfRouter];
  }
}

void
RoutingTableCalculator::initMatrix()
{
  for (int i = 0; i < numOfRouter; i++)
  {
    for (int j = 0; j < numOfRouter; j++)
      adjMatrix[i][j] = 0;
  }
}

void
RoutingTableCalculator::makeAdjMatrix(Nlsr& pnlsr, Map pMap)
{
  std::list<AdjLsa> adjLsdb = pnlsr.getLsdb().getAdjLsdb();
  for (std::list<AdjLsa>::iterator it = adjLsdb.begin();
       it != adjLsdb.end() ; it++)
  {
    string linkStartRouter = (*it).getOrigRouter();
    int row = pMap.getMappingNoByRouterName(linkStartRouter);
    std::list<Adjacent> adl = (*it).getAdl().getAdjList();
    for (std::list<Adjacent>::iterator itAdl = adl.begin();
         itAdl != adl.end() ; itAdl++)
    {
      string linkEndRouter = (*itAdl).getName();
      int col = pMap.getMappingNoByRouterName(linkEndRouter);
      double cost = (*itAdl).getLinkCost();
      if ((row >= 0 && row < numOfRouter) && (col >= 0 && col < numOfRouter))
      {
        adjMatrix[row][col] = cost;
      }
    }
  }
}

void
RoutingTableCalculator::printAdjMatrix()
{
  for (int i = 0; i < numOfRouter; i++)
  {
    for (int j = 0; j < numOfRouter; j++)
      printf("%f ", adjMatrix[i][j]);
    printf("\n");
  }
}

void
RoutingTableCalculator::adjustAdMatrix(int source, int link, double linkCost)
{
  for (int i = 0; i < numOfRouter; i++)
  {
    if (i == link)
    {
      adjMatrix[source][i] = linkCost;
    }
    else
    {
      adjMatrix[source][i] = 0;
    }
  }
}

int
RoutingTableCalculator::getNumOfLinkfromAdjMatrix(int sRouter)
{
  int noLink = 0;
  for (int i = 0; i < numOfRouter; i++)
  {
    if (adjMatrix[sRouter][i] > 0)
    {
      noLink++;
    }
  }
  return noLink;
}

void
RoutingTableCalculator::getLinksFromAdjMatrix(int* links,
                                              double* linkCosts, int source)
{
  int j = 0;
  for (int i = 0; i < numOfRouter; i++)
  {
    if (adjMatrix[source][i] > 0)
    {
      links[j] = i;
      linkCosts[j] = adjMatrix[source][i];
      j++;
    }
  }
}

void
RoutingTableCalculator::freeAdjMatrix()
{
  for (int i = 0; i < numOfRouter; ++i)
  {
    delete [] adjMatrix[i];
  }
  delete [] adjMatrix;
}


void
RoutingTableCalculator::allocateLinks()
{
  links = new int[vNoLink];
}

void
RoutingTableCalculator::allocateLinkCosts()
{
  linkCosts = new double[vNoLink];
}


void
RoutingTableCalculator::freeLinks()
{
  delete [] links;
}
void
RoutingTableCalculator::freeLinksCosts()
{
  delete [] linkCosts;
}

void
LinkStateRoutingTableCalculator::calculatePath(Map& pMap,
                                               RoutingTable& rt, Nlsr& pnlsr)
{
  std::cout << "LinkStateRoutingTableCalculator::calculatePath Called" <<
            std::endl;
  allocateAdjMatrix();
  initMatrix();
  makeAdjMatrix(pnlsr, pMap);
  std::cout << pMap;
  printAdjMatrix();
  string routerName = pnlsr.getConfParameter().getRouterPrefix();
  int sourceRouter = pMap.getMappingNoByRouterName(routerName);
  //int noLink=getNumOfLinkfromAdjMatrix(sourceRouter);
  allocateParent();
  allocateDistance();
  if (pnlsr.getConfParameter().getMaxFacesPerPrefix() == 1)
  {
    // Single Path
    doDijkstraPathCalculation(sourceRouter);
    // print all ls path -- debugging purpose
    printAllLsPath(sourceRouter);
    // update routing table
    addAllLsNextHopsToRoutingTable(pnlsr, rt, pMap, sourceRouter);
  }
  else
  {
    // Multi Path
    setNoLink(getNumOfLinkfromAdjMatrix(sourceRouter));
    allocateLinks();
    allocateLinkCosts();
    getLinksFromAdjMatrix(links, linkCosts, sourceRouter);
    for (int i = 0 ; i < vNoLink; i++)
    {
      adjustAdMatrix(sourceRouter, links[i], linkCosts[i]);
      printAdjMatrix();
      doDijkstraPathCalculation(sourceRouter);
      // print all ls path -- debugging purpose
      printAllLsPath(sourceRouter);
      //update routing table
      addAllLsNextHopsToRoutingTable(pnlsr, rt, pMap, sourceRouter);
    }
    freeLinks();
    freeLinksCosts();
  }
  freeParent();
  freeDistance();
  freeAdjMatrix();
}

void
LinkStateRoutingTableCalculator::doDijkstraPathCalculation(int sourceRouter)
{
  int i;
  int v, u;
  int* Q = new int[numOfRouter];
  int head = 0;
  /* Initiate the Parent */
  for (i = 0 ; i < numOfRouter; i++)
  {
    m_parent[i] = EMPTY_PARENT;
    m_distance[i] = INF_DISTANCE;
    Q[i] = i;
  }
  if (sourceRouter != NO_MAPPING_NUM)
  {
    m_distance[sourceRouter] = 0;
    sortQueueByDistance(Q, m_distance, head, numOfRouter);
    while (head < numOfRouter)
    {
      u = Q[head];
      if (m_distance[u] == INF_DISTANCE)
      {
        break;
      }
      for (v = 0 ; v < numOfRouter; v++)
      {
        if (adjMatrix[u][v] > 0)
        {
          if (isNotExplored(Q, v, head + 1, numOfRouter))
          {
            if (m_distance[u] + adjMatrix[u][v] <  m_distance[v])
            {
              m_distance[v] = m_distance[u] + adjMatrix[u][v] ;
              m_parent[v] = u;
            }
          }
        }
      }
      head++;
      sortQueueByDistance(Q, m_distance, head, numOfRouter);
    }
  }
  delete [] Q;
}

void
LinkStateRoutingTableCalculator::addAllLsNextHopsToRoutingTable(Nlsr& pnlsr,
                                                                RoutingTable& rt, Map& pMap, int sourceRouter)
{
  std::cout <<
            "LinkStateRoutingTableCalculator::addAllNextHopsToRoutingTable Called";
  std::cout << std::endl;
  int nextHopRouter = 0;
  for (int i = 0; i < numOfRouter ; i++)
  {
    if (i != sourceRouter)
    {
      nextHopRouter = getLsNextHop(i, sourceRouter);
      if (nextHopRouter != NO_NEXT_HOP)
      {
        double routeCost = m_distance[i];
        string nextHopRouterName =
          pMap.getRouterNameByMappingNo(nextHopRouter);
        int nxtHopFace =
          pnlsr.getAdl().getAdjacent(nextHopRouterName).getConnectingFace();
        std::cout << "Dest Router: " << pMap.getRouterNameByMappingNo(i) << std::endl;
        std::cout << "Next hop Router: " << nextHopRouterName << std::endl;
        std::cout << "Next hop Face: " << nxtHopFace << std::endl;
        std::cout << "Route Cost: " << routeCost << std::endl;
        std::cout << std::endl;
        // Add next hop to routing table
        NextHop nh(nxtHopFace, routeCost);
        rt.addNextHop(pMap.getRouterNameByMappingNo(i), nh);
      }
    }
  }
}

int
LinkStateRoutingTableCalculator::getLsNextHop(int dest, int source)
{
  int nextHop = NO_NEXT_HOP;
  while (m_parent[dest] != EMPTY_PARENT)
  {
    nextHop = dest;
    dest = m_parent[dest];
  }
  if (dest != source)
  {
    nextHop = NO_NEXT_HOP;
  }
  return nextHop;
}

void
LinkStateRoutingTableCalculator::printAllLsPath(int sourceRouter)
{
  std::cout << "LinkStateRoutingTableCalculator::printAllLsPath Called" <<
            std::endl;
  std::cout << "Source Router: " << sourceRouter << std::endl;
  for (int i = 0; i < numOfRouter ; i++)
  {
    if (i != sourceRouter)
    {
      printLsPath(i);
      std::cout << std::endl;
    }
  }
}

void
LinkStateRoutingTableCalculator::printLsPath(int destRouter)
{
  if (m_parent[destRouter] != EMPTY_PARENT)
  {
    printLsPath(m_parent[destRouter]);
  }
  std:: cout << " " << destRouter;
}

void
LinkStateRoutingTableCalculator::sortQueueByDistance(int* Q,
                                                     double* dist, int start, int element)
{
  for (int i = start ; i < element ; i++)
  {
    for (int j = i + 1; j < element; j++)
    {
      if (dist[Q[j]] < dist[Q[i]])
      {
        int tempU = Q[j];
        Q[j] = Q[i];
        Q[i] = tempU;
      }
    }
  }
}

int
LinkStateRoutingTableCalculator::isNotExplored(int* Q,
                                               int u, int start, int element)
{
  int ret = 0;
  for (int i = start; i < element; i++)
  {
    if (Q[i] == u)
    {
      ret = 1;
      break;
    }
  }
  return ret;
}

void
LinkStateRoutingTableCalculator::allocateParent()
{
  m_parent = new int[numOfRouter];
}

void
LinkStateRoutingTableCalculator::allocateDistance()
{
  m_distance = new double[numOfRouter];
}

void
LinkStateRoutingTableCalculator::freeParent()
{
  delete [] m_parent;
}

void LinkStateRoutingTableCalculator::freeDistance()
{
  delete [] m_distance;
}



void
HypRoutingTableCalculator::calculatePath(Map& pMap,
                                         RoutingTable& rt, Nlsr& pnlsr)
{
  makeAdjMatrix(pnlsr, pMap);
  string routerName = pnlsr.getConfParameter().getRouterPrefix();
  int sourceRouter = pMap.getMappingNoByRouterName(routerName);
  int noLink = getNumOfLinkfromAdjMatrix(sourceRouter);
  setNoLink(noLink);
  allocateLinks();
  allocateLinkCosts();
  getLinksFromAdjMatrix(links, linkCosts, sourceRouter);
  for (int i = 0 ; i < numOfRouter ; ++i)
  {
    int k = 0;
    if (i != sourceRouter)
    {
      allocateLinkFaces();
      allocateDistanceToNeighbor();
      allocateDistFromNbrToDest();
      for (int j = 0; j < vNoLink; j++)
      {
        string nextHopRouterName = pMap.getRouterNameByMappingNo(links[j]);
        int nextHopFace =
          pnlsr.getAdl().getAdjacent(nextHopRouterName).getConnectingFace();
        double distToNbr = getHyperbolicDistance(pnlsr, pMap,
                                                 sourceRouter, links[j]);
        double distToDestFromNbr = getHyperbolicDistance(pnlsr,
                                                         pMap, links[j], i);
        if (distToDestFromNbr >= 0)
        {
          m_linkFaces[k] = nextHopFace;
          m_distanceToNeighbor[k] = distToNbr;
          m_distFromNbrToDest[k] = distToDestFromNbr;
          k++;
        }
      }
      addHypNextHopsToRoutingTable(pnlsr, pMap, rt, k, i);
      freeLinkFaces();
      freeDistanceToNeighbor();
      freeDistFromNbrToDest();
    }
  }
  freeLinks();
  freeLinksCosts();
  freeAdjMatrix();
}

void
HypRoutingTableCalculator::addHypNextHopsToRoutingTable(Nlsr& pnlsr, Map& pMap,
                                                        RoutingTable& rt, int noFaces, int dest)
{
  for (int i = 0 ; i < noFaces ; ++i)
  {
    string destRouter = pMap.getRouterNameByMappingNo(dest);
    NextHop nh(m_linkFaces[i], m_distFromNbrToDest[i]);
    rt.addNextHop(destRouter, nh);
    if (m_isDryRun)
    {
      rt.addNextHopToDryTable(destRouter, nh);
    }
  }
}

double
HypRoutingTableCalculator::getHyperbolicDistance(Nlsr& pnlsr,
                                                 Map& pMap, int src, int dest)
{
  double distance = 0.0;
  string srcRouterKey = pMap.getRouterNameByMappingNo(src) + "/3";
  string destRouterKey = pMap.getRouterNameByMappingNo(dest) + "/3";
  double srcRadius = (pnlsr.getLsdb().findCoordinateLsa(
                        srcRouterKey))->getCorRadius();
  double srcTheta = (pnlsr.getLsdb().findCoordinateLsa(
                       srcRouterKey))->getCorTheta();
  double destRadius = (pnlsr.getLsdb().findCoordinateLsa(
                         destRouterKey))->getCorRadius();
  double destTheta = (pnlsr.getLsdb().findCoordinateLsa(
                        destRouterKey))->getCorTheta();
  double diffTheta = fabs(srcTheta - destTheta);
  if (diffTheta > MATH_PI)
  {
    diffTheta = 2 * MATH_PI - diffTheta;
  }
  if (srcRadius != -1 && destRadius != -1)
  {
    if (diffTheta == 0)
      distance = fabs(srcRadius - destRadius);
    else
      distance = acosh((cosh(srcRadius) * cosh(destRadius)) -
                       (sinh(srcRadius) * sinh(destRadius) * cos(diffTheta)));
  }
  else
  {
    distance = -1;
  }
  return distance;
}

void
HypRoutingTableCalculator::allocateLinkFaces()
{
  m_linkFaces = new int[vNoLink];
}

void
HypRoutingTableCalculator::allocateDistanceToNeighbor()
{
  m_distanceToNeighbor = new double[vNoLink];
}

void
HypRoutingTableCalculator::allocateDistFromNbrToDest()
{
  m_distFromNbrToDest = new double[vNoLink];
}

void
HypRoutingTableCalculator::freeLinkFaces()
{
  delete [] m_linkFaces;
}

void
HypRoutingTableCalculator::freeDistanceToNeighbor()
{
  delete [] m_distanceToNeighbor;
}

void
HypRoutingTableCalculator::freeDistFromNbrToDest()
{
  delete [] m_distFromNbrToDest;
}

}//namespace nlsr