import React, { useState, useEffect } from 'react';
import { Card, CardContent, CardDescription, CardFooter, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { Button } from "@/components/ui/button";
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select";
import { Label } from "@/components/ui/label";
import { Alert, AlertDescription } from "@/components/ui/alert";
import { Eye, EyeOff, Lock, Loader2, Wifi, RefreshCw } from "lucide-react";

const CaptivePortal: React.FC = () => {
  // State management
  const [connectionStatus, setConnectionStatus] = useState<'disconnected' | 'connecting' | 'connected'>('disconnected');
  const [ssid, setSsid] = useState("");
  const [password, setPassword] = useState("");
  const [showPassword, setShowPassword] = useState(false);
  const [availableNetworks, setAvailableNetworks] = useState<string[]>([]);
  const [scanning, setScanning] = useState(true);
  const [manualInput, setManualInput] = useState(false);
  
  // Scan for networks effect
  useEffect(() => {
    scanNetworks();
  }, []);
  
  // Function to scan for networks - in a real implementation, this would
  // make an API call to the ESP32 to get the list of networks
  const scanNetworks = () => {
    setScanning(true);
    setAvailableNetworks([]);
    
    // Simulating network scan - in real implementation this would be
    // replaced with an actual API call to the ESP32
    fetch('/scan')
      .then(response => response.json())
      .then(networks => {
        setAvailableNetworks(networks);
        setScanning(false);
      })
      .catch(() => {
        // If API call fails, set sample networks for demo
        setTimeout(() => {
          setAvailableNetworks([]);
          setScanning(false);
        }, 1500);
      });
  };
  
  // Handle form submission
  const handleConnect = (e: React.FormEvent) => {
    e.preventDefault();
    if (ssid && password) {
      setConnectionStatus('connecting');
      
      // In real implementation, this would be an API call to send credentials
      // to the ESP32 using AsyncWebServer
      const formData = new FormData();
      formData.append('ssid', ssid);
      formData.append('password', password);
      
      fetch('/connect', {
        method: 'POST',
        body: formData
      })
        .then(() => {
          setTimeout(() => setConnectionStatus('connected'), 2000);
        })
        .catch(error => {
          console.error('Connection error:', error);
          setConnectionStatus('disconnected');
        });
    }
  };
  
  return (
    <div className="gradient-bg flex items-center justify-center p-4 min-h-screen">
      <Card className="w-full max-w-md shadow-xl">
        <CardHeader className="space-y-1">
          <div className="flex items-center justify-center mb-4 text-primary">
            <Wifi size={32} />
          </div>
          <CardTitle className="text-2xl font-bold text-center">Power Hub Setup</CardTitle>
          <CardDescription className="text-center">
            Connect this device to your home WiFi network
          </CardDescription>
        </CardHeader>
        
        <CardContent className="space-y-4">
          <form onSubmit={handleConnect} className="space-y-4">
            <div className="space-y-2">
              <Label htmlFor="network">WiFi Network</Label>
              
              {!manualInput ? (
                <>
                  <div className="flex gap-2">
                    <Select 
                      onValueChange={(value) => {
                        setSsid(value);
                        setManualInput(value === "manual-input");
                      }}
                      disabled={connectionStatus === 'connecting' || scanning}
                      value={ssid}
                    >
                      <SelectTrigger id="network" className="w-full">
                        <SelectValue placeholder={scanning ? "Scanning..." : "Select network..."} />
                      </SelectTrigger>
                      <SelectContent>
                        {availableNetworks.length > 0 ? (
                          availableNetworks.map((network) => (
                            <SelectItem key={network} value={network}>
                              <div className="flex items-center">
                                <Wifi className="h-4 w-4 mr-2 text-primary" />
                                {network}
                              </div>
                            </SelectItem>
                          ))
                        ) : (
                          <SelectItem value="manual-input">
                            No networks found - Enter manually
                          </SelectItem>
                        )}
                      </SelectContent>
                    </Select>
                    
                    <Button 
                      type="button" 
                      variant="outline" 
                      size="icon" 
                      onClick={scanNetworks}
                      disabled={connectionStatus === 'connecting' || scanning}
                    >
                      <RefreshCw className={`h-4 w-4 ${scanning ? 'animate-spin' : ''}`} />
                    </Button>
                  </div>
                  
                  {scanning && (
                    <div className="text-xs text-muted-foreground flex items-center gap-2 mt-1">
                      <Loader2 className="h-3 w-3 animate-spin" />
                      Scanning for networks...
                    </div>
                  )}
                </>
              ) : (
                <div className="space-y-2">
                  <Input 
                    id="manual-network"
                    type="text"
                    value={ssid}
                    onChange={(e) => setSsid(e.target.value)}
                    placeholder="Enter WiFi name"
                    disabled={connectionStatus === 'connecting'}
                  />
                  <div className="flex justify-end">
                    <Button 
                      type="button" 
                      variant="link" 
                      size="sm" 
                      onClick={() => {
                        setManualInput(false);
                        setSsid('');
                        scanNetworks();
                      }}
                      className="p-0 h-auto text-xs"
                    >
                      Back to network list
                    </Button>
                  </div>
                </div>
              )}
            </div>
            
            <div className="space-y-2">
              <Label htmlFor="password">Password</Label>
              <div className="relative">
                <div className="absolute inset-y-0 left-0 flex items-center pl-3 pointer-events-none">
                  <Lock className="h-4 w-4 text-gray-400" />
                </div>
                <Input
                  id="password"
                  type={showPassword ? "text" : "password"}
                  value={password}
                  onChange={(e) => setPassword(e.target.value)}
                  placeholder="WiFi password"
                  className="pl-10 pr-10"
                  required
                  disabled={connectionStatus === 'connecting' || !ssid}
                />
                <div className="absolute inset-y-0 right-0 flex items-center pr-3">
                  <Button
                    type="button"
                    variant="ghost"
                    size="sm"
                    className="h-4 w-4 p-0"
                    onClick={() => setShowPassword(!showPassword)}
                    disabled={connectionStatus === 'connecting' || !ssid}
                  >
                    {showPassword ? <EyeOff className="h-4 w-4" /> : <Eye className="h-4 w-4" />}
                  </Button>
                </div>
              </div>
            </div>
            
            <Button 
              type="submit" 
              className="w-full" 
              disabled={connectionStatus === 'connecting' || !ssid || !password}
            >
              {connectionStatus === 'connecting' ? (
                <>
                  <Loader2 className="mr-2 h-4 w-4 animate-spin" />
                  Connecting...
                </>
              ) : (
                'Connect'
              )}
            </Button>
          </form>
          
          {connectionStatus === 'connected' && (
            <Alert className="bg-green-50 text-green-700 border-green-200">
              <div className="flex items-center space-x-2">
                <span className="connection-dot connected"></span>
                <AlertDescription>
                  Connected successfully! Device is now online.
                </AlertDescription>
              </div>
            </Alert>
          )}
        </CardContent>
        
        <CardFooter className="flex flex-col space-y-2 text-center text-sm text-muted-foreground">
          <p>This device needs internet access to function properly.</p>
          <p className="text-xs">Your credentials are only stored locally on this device.</p>
        </CardFooter>
      </Card>
    </div>
  );
};

export default CaptivePortal;
