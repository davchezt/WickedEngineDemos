#include "stdafx.h"
#include "WiDemo.h"
#include "wiProfiler.h"

using namespace std;
using namespace wiGraphicsTypes;

Demo::Demo()
{
	MainComponent::MainComponent();
	
	demoScene = HELLOWORLD;
	interactionType = SPAWN_OBJECT;

	setFrameSkip(true);

}
Demo::~Demo()
{
	wiRenderer::CleanUpStatic();
	wiImage::CleanUp();
	wiFont::CleanUpStatic();
	wiLensFlare::CleanUp();
}
void Demo::Initialize()
{
	MainComponent::Initialize();

	wiInitializer::InitializeComponents(
		wiInitializer::WICKEDENGINE_INITIALIZE_RENDERER
		| wiInitializer::WICKEDENGINE_INITIALIZE_IMAGE
		| wiInitializer::WICKEDENGINE_INITIALIZE_FONT
		| wiInitializer::WICKEDENGINE_INITIALIZE_SOUND
		| wiInitializer::WICKEDENGINE_INITIALIZE_MISC
		);

	wiRenderer::GetDevice()->SetVSyncEnabled(false);
	wiRenderer::EMITTERSENABLED = true;
	wiRenderer::HAIRPARTICLEENABLED = true;
	//wiRenderer::DX11 = false;
	wiRenderer::physicsEngine = new wiBULLET();

	wiRenderer::SetOcclusionCullingEnabled(false);
	wiRenderer::SetAdvancedRefractionsEnabled(false);

#ifdef BUILD_PHONE
	wiRenderer::SetShadowProps2D(256, 4, 0);
	wiRenderer::SetShadowPropsCube(128, 1);
#endif

	wiProfiler::GetInstance().ENABLED = false;

	//wiInputManager::GetInstance()->addDirectInput(new wiDirectInput(instance, window));
	wiInputManager::GetInstance()->addXInput(new wiXInput());
	//wiInputManager::GetInstance()->addRawInput(new wiRawInput(window));

	Content.add("sound/change.wav", wiResourceManager::SOUND);
	wiSoundEffect::SetVolume(0.5f);

	CameraReset();

	demos.insert(pair<DEMOS, RenderableComponent*>(LOADINGSCREEN, new DemoLoadingScreen()));
	demos.insert(pair<DEMOS, RenderableComponent*>(HELLOWORLD, new HelloWorldDemo()));
	demos.insert(pair<DEMOS, RenderableComponent*>(BASICMODEL, new BasicModelDemo()));
	demos.insert(pair<DEMOS, RenderableComponent*>(SKINNEDMODEL, new SkinnedModelDemo()));
	demos.insert(pair<DEMOS, RenderableComponent*>(EMITTERPARTICLE, new EmittedParticleDemo()));
	demos.insert(pair<DEMOS, RenderableComponent*>(HAIRPARTICLE, new HairParticleDemo()));
	demos.insert(pair<DEMOS, RenderableComponent*>(RIGIDBODY, new RigidBodyDemo()));
	demos.insert(pair<DEMOS, RenderableComponent*>(SOFTBODY, new SoftBodyDemo()));
	demos.insert(pair<DEMOS, RenderableComponent*>(DEFERREDSCENE, new DeferredSceneDemo()));
	demos.insert(pair<DEMOS, RenderableComponent*>(DEFERREDLIGHTS, new DeferredLightDemo()));
	demos.insert(pair<DEMOS, RenderableComponent*>(SSRTEST, new SSRTestDemo()));
	demos.insert(pair<DEMOS, RenderableComponent*>(FORWARDSCENE, new ForwardSceneDemo()));
	demos.insert(pair<DEMOS, RenderableComponent*>(SKINTEST, new SkinTestDemo()));

	for (pair<DEMOS, RenderableComponent*> x : demos)
	{
		x.second->Initialize();
	}

	demos[LOADINGSCREEN]->Load();

	ChangeDemo(DEFERREDSCENE);
}
void Demo::Update(float dt)
{
	CameraControl(dt);

	if (!wiBackLog::isActive())
	{
		if (wiInputManager::GetInstance()->press('1')){
			ChangeDemo(HELLOWORLD);
		}
		else if (wiInputManager::GetInstance()->press('2')){
			ChangeDemo(BASICMODEL);
		}
		else if (wiInputManager::GetInstance()->press('3')){
			ChangeDemo(SKINNEDMODEL);
		}
		else if (wiInputManager::GetInstance()->press('4')){
			ChangeDemo(EMITTERPARTICLE);
		}
		else if (wiInputManager::GetInstance()->press('5')){
			ChangeDemo(HAIRPARTICLE);
		}
		else if (wiInputManager::GetInstance()->press('6')){
			ChangeDemo(RIGIDBODY);
		}
		else if (wiInputManager::GetInstance()->press('7')){
			ChangeDemo(SOFTBODY);
		}
		else if (wiInputManager::GetInstance()->press('8')){
			ChangeDemo(DEFERREDLIGHTS);
		}
		else if (wiInputManager::GetInstance()->press('9')){
			ChangeDemo(DEFERREDSCENE);
		}
		else if (wiInputManager::GetInstance()->press('0')){
			ChangeDemo(SSRTEST);
		}
		else if (wiInputManager::GetInstance()->press(VK_F1)) {
			ChangeDemo(FORWARDSCENE);
		}
		else if (wiInputManager::GetInstance()->press(VK_F2)) {
			ChangeDemo(SKINTEST);
		}
		else if (wiInputManager::GetInstance()->press(XINPUT_GAMEPAD_DPAD_LEFT, wiInputManager::XINPUT_JOYPAD, 0))
		{
			ChangeDemo((DEMOS)(demoScene - 1));
		}
		else if (wiInputManager::GetInstance()->press(XINPUT_GAMEPAD_DPAD_RIGHT, wiInputManager::XINPUT_JOYPAD, 0))
		{
			ChangeDemo((DEMOS)(demoScene + 1));
		}
		else if (wiInputManager::GetInstance()->press(POV_LEFT, wiInputManager::DIRECTINPUT_JOYPAD, 0))
		{
			ChangeDemo((DEMOS)(demoScene - 1));
		}
		else if (wiInputManager::GetInstance()->press(POV_RIGHT, wiInputManager::DIRECTINPUT_JOYPAD, 0))
		{
			ChangeDemo((DEMOS)(demoScene + 1));
		}
	}
	else
	{
		if (wiInputManager::GetInstance()->press(VK_PRIOR) || wiInputManager::GetInstance()->hold(VK_PRIOR, 30, true)){ //page up
			wiBackLog::Scroll(-5);
		}
		else if (wiInputManager::GetInstance()->press(VK_NEXT) || wiInputManager::GetInstance()->hold(VK_NEXT, 40, true)){ //page down
			wiBackLog::Scroll(5);
		}
	}
	if (wiInputManager::GetInstance()->press(VK_HOME)){
		wiBackLog::Toggle();
	}

	MainComponent::Update(dt);
}
void Demo::Render()
{
	MainComponent::Render();

	HudRender();
}

void Demo::CameraControl(float dt){

	if (wiBackLog::isActive())
		return;

	float speed = (wiInputManager::GetInstance()->down(VK_SHIFT) ? 100.0f : 30.0f) * dt;
	if (wiInputManager::GetInstance()->down('A')) wiRenderer::getCamera()->Move(XMVectorSet(-speed, 0, 0, 0));
	if (wiInputManager::GetInstance()->down('D')) wiRenderer::getCamera()->Move(XMVectorSet(speed, 0, 0, 0));
	if (wiInputManager::GetInstance()->down('W')) wiRenderer::getCamera()->Move(XMVectorSet(0, 0, speed, 0));
	if (wiInputManager::GetInstance()->down('S')) wiRenderer::getCamera()->Move(XMVectorSet(0, 0, -speed, 0));
	if (wiInputManager::GetInstance()->down(VK_SPACE)) wiRenderer::getCamera()->Move(XMVectorSet(0, speed, 0, 0));
	if (wiInputManager::GetInstance()->down(VK_CONTROL)) wiRenderer::getCamera()->Move(XMVectorSet(0, -speed, 0, 0));

	static XMFLOAT4 originalMouse = XMFLOAT4(0, 0, 0, 0);
	XMFLOAT4 currentMouse = wiInputManager::GetInstance()->getpointer();
	if (wiInputManager::GetInstance()->down(VK_RBUTTON) || mousebuttondown)
	{
		float xDif = currentMouse.x - originalMouse.x;
		float yDif = currentMouse.y - originalMouse.y;
		wiRenderer::getCamera()->RotateRollPitchYaw(XMFLOAT3(0, xDif*dt, 0));
		wiRenderer::getCamera()->RotateRollPitchYaw(XMFLOAT3(yDif*dt, 0, 0));
		wiInputManager::GetInstance()->setpointer(originalMouse);
	}
	else
	{
		originalMouse = wiInputManager::GetInstance()->getpointer();
	}


	// TOUCH

	static XMFLOAT4 originalMouseL = XMFLOAT4(0, 0, 0, 0);
	static XMFLOAT4 originalMouseR = XMFLOAT4(0, 0, 0, 0);
	auto& touches = wiInputManager::GetInstance()->getTouches();
	for (auto& touch : touches)
	{
		switch (touch.state)
		{
		case wiInputManager::Touch::TOUCHSTATE_PRESSED:
		{
			if (touch.pos.x < screenW / 4 && touch.pos.y < screenH / 4)
			{
				DEMOS nextDemo = (DEMOS)(demoScene + 1);
				if (nextDemo == DEMO_COUNT)
					nextDemo = HELLOWORLD;
				ChangeDemo(nextDemo);
			}
			else if (touch.pos.x > screenW * 0.75f && touch.pos.y < screenH / 4)
			{
				XMMATRIX spawnTrans = XMMatrixRotationQuaternion(XMLoadFloat4(&wiRenderer::getCamera()->rotation))*XMMatrixTranslationFromVector(XMVectorAdd(wiRenderer::getCamera()->GetEye(), wiRenderer::getCamera()->GetAt() * 5));
				switch (wiRandom::getRandom(0, 3))
				{
				case 0:
					wiRenderer::LoadModel("models/barrel/", "barrel",
						spawnTrans
						, "common");
					break;
				case 1:
					wiRenderer::LoadModel("models/crate/", "crate",
						spawnTrans
						, "common");
					break;
				case 2:
					wiRenderer::LoadModel("models/monkey/", "monkey",
						spawnTrans
						, "common"
					);
					break;
				case 3:
					wiRenderer::LoadModel("models/golfball/", "golfball",
						spawnTrans
						, "common"
					);
					break;
				default:
					break;
				}
			}
			else
			{
				if (touch.pos.x < screenW / 2)
				{
					originalMouseL = XMFLOAT4(touch.pos.x, touch.pos.y, 0, 0);
				}
				else
				{
					originalMouseR = XMFLOAT4(touch.pos.x, touch.pos.y, 0, 0);
				}
			}
		}
		break;
		case wiInputManager::Touch::TOUCHSTATE_MOVED:
		{
			if (touch.pos.x < screenW / 2)
			{
				float speed = -0.5f * dt;
				XMFLOAT4 currentMouse = XMFLOAT4(touch.pos.x, touch.pos.y, 0, 0);
				float xDif = originalMouseL.x - currentMouse.x;
				float yDif = originalMouseL.y - currentMouse.y;
				wiRenderer::getCamera()->Move(XMVectorSet(xDif, 0, -yDif, 0)*speed);
			}
			else
			{
				XMFLOAT4 currentMouse = XMFLOAT4(touch.pos.x, touch.pos.y, 0, 0);
				float xDif = originalMouseR.x - currentMouse.x;
				float yDif = originalMouseR.y - currentMouse.y;
				wiRenderer::getCamera()->RotateRollPitchYaw(XMFLOAT3(0, 0.1f*dt*xDif, 0));
				wiRenderer::getCamera()->RotateRollPitchYaw(XMFLOAT3(0.1f*dt*yDif, 0, 0));
				originalMouseR = currentMouse;
			}
		}
		break;
		default:
			break;
		}
	}
}
void Demo::CameraReset(){
	wiRenderer::getCamera()->Clear();
	wiRenderer::getCamera()->transform(XMFLOAT3(0, 3, -4));
}
void Demo::StartLoadingChangeDemo(DEMOS newDemo){
	CameraReset();
	wiRenderer::CleanUpStaticTemp();
	demos[newDemo]->Load();
}
void Demo::FinishLoadingChangeDemo(DEMOS newDemo)
{
	demoScene = newDemo;
	activateComponent(demos[demoScene]);
}
void Demo::ChangeDemo(DEMOS newDemo){
	if (demoScene == LOADINGSCREEN || newDemo==LOADINGSCREEN)
	{
		return;
	}

	wiRenderer::CleanUpStaticTemp();

	static_cast<wiSoundEffect*>(Content.get("sound/change.wav")->data)->Play();

	if (demos.find(newDemo) != demos.end()){
		LoadingScreenComponent* loading = dynamic_cast<LoadingScreenComponent*>(demos[LOADINGSCREEN]);
		if (loading != nullptr)
		{
			loading->Stop();
			for (int i = 0; i < 10; ++i)
			{
				//Added some fake loading simulation tasks
				//just to see it visually, because these demos load in one task
				loading->addLoadingFunction(bind(Sleep, i * 20));
			}
			loading->addLoadingFunction(bind(&Demo::StartLoadingChangeDemo, this, newDemo));
			loading->onFinished(bind(&Demo::FinishLoadingChangeDemo, this, newDemo));
			demoScene = LOADINGSCREEN;
			activateComponent(demos[demoScene]);
		}
	}

}
void Demo::HudRender(){
	if (demoScene == LOADINGSCREEN)
		return;

	Renderable3DComponent* r3c = dynamic_cast<Renderable3DComponent*>(getActiveComponent());
	if (r3c != nullptr && r3c->getStereogramEnabled())
		return;

	stringstream ss("");
	ss << "Wicked Engine v" << wiVersion::GetVersionString();
#ifdef _DEBUG
	ss << " [DEBUG]";
#endif
	ss << "\nResolution: " << wiRenderer::GetDevice()->GetScreenWidth() << " x " << wiRenderer::GetDevice()->GetScreenHeight();
	ss << "\nDeferred context support: " << (wiRenderer::GetDevice()->CheckCapability(GraphicsDevice::GRAPHICSDEVICE_CAPABILITY_MULTITHREADED_RENDERING) ? "yes" : "no");
	XMFLOAT4 po = wiInputManager::GetInstance()->getpointer();
	ss << "\nPointer: " << po.x << "," << po.y;
	ss << "\n\nDemo Select:\n------------------";
	ss << "\n[1] :  HelloWorld";
	ss << "\n[2] :  BasicModel";
	ss << "\n[3] :  SkinnedModel";
	ss << "\n[4] :  EmitterParticle";
	ss << "\n[5] :  HairParticle";
	ss << "\n[6] :  RigidBody";
	ss << "\n[7] :  SoftBody";
	ss << "\n[8] :  DeferredLights";
	ss << "\n[9] :  DeferredScene";
	ss << "\n[0] :  SSRTest";
	ss << "\n[F1] : ForwardScene";
	ss << "\n\nControls:\n-----------------\nMove with WASD\nLook with RMB\nChange interaction type with MOUSEWHEEL\nHOME: Toggle console";
	ss << "\nInteraction type: ";
	switch (interactionType)
	{
	case DECAL:
		ss << "DECAL";
		break;
	case WATER:
		ss << "WATER";
		break;
	case SPAWN_OBJECT:
		ss << "SPAWN OBJECT";
		break;
	case SPAWN_LIGHT:
		ss << "SPAWN LIGHT";
		break;
	default:
		break;
	}
	wiFont(ss.str(), wiFontProps(0, 0, -1, WIFALIGN_LEFT, WIFALIGN_TOP)).Draw(GRAPHICSTHREAD_IMMEDIATE);
	ss.str("");
	ss.precision(1);
	ss << fixed << wiFrameRate::FPS() << " FPS";
	ss << "\nCPU: " << wiCpuInfo::GetCpuPercentage() << "%";
	//ss << "\nRAWInput Joy: " << wiInputManager::rawinput->raw.data.hid.bRawData[0];
	//ss << "\nRAWInput Keyboard: " << (char)wiInputManager::rawinput->raw.data.keyboard.VKey;
	//ss << "\nRAWInput Mouse: " << wiInputManager::rawinput->raw.data.mouse.lLastX << ":" << wiInputManager::rawinput->raw.data.mouse.lLastY;
	wiFont(ss.str(), wiFontProps(wiRenderer::GetDevice()->GetScreenWidth() - 20, 0, -1, WIFALIGN_RIGHT, WIFALIGN_TOP)).Draw(GRAPHICSTHREAD_IMMEDIATE);
	ss.str("");
	switch (demoScene)
	{
	case HELLOWORLD:
		ss << "HELLOWORLD";
		break;
	case BASICMODEL:
		ss << "BASICMODEL";
		break;
	case SKINNEDMODEL:
		ss << "SKINNEDMODEL";
		break;
	case EMITTERPARTICLE:
		ss << "EMITTERPARTICLE";
		break;
	case HAIRPARTICLE:
		ss << "HAIRPARTICLE";
		break;
	case RIGIDBODY:
		ss << "RIGIDBODY";
		break;
	case SOFTBODY:
		ss << "SOFTBODY";
		break;
	case DEFERREDSCENE:
		ss << "DEFERREDSCENE";
		break;
	case DEFERREDLIGHTS:
		ss << "DEFERREDLIGHTS";
		break;
	case FORWARDSCENE:
		ss << "FORWARDSCENE";
		break;
	case SSRTEST:
		ss << "SSRTEST";
		break;
	default:
		break;
	}
	ss << " DEMO";
	wiFont(ss.str(), wiFontProps(wiRenderer::GetDevice()->GetScreenWidth() / 2, wiRenderer::GetDevice()->GetScreenHeight(), -1, WIFALIGN_CENTER, WIFALIGN_BOTTOM)).Draw(GRAPHICSTHREAD_IMMEDIATE);
	//wiFont::Draw(ss.str(), "basic", XMFLOAT4((float)wiRenderer::GetDevice()->GetScreenWidth() / 2, -(float)wiRenderer::GetDevice()->GetScreenHeight(), -5, -4), "center", "bottom");
}


void DemoLoadingScreen::Load()
{
	wiSprite* sprite = new wiSprite();
	sprite->setTexture(wiTextureHelper::getInstance()->getWhite());
	sprite->anim.rot = 0.08f;
	sprite->effects.siz = XMFLOAT2(100.f, 100.f);
	sprite->effects.pos = XMFLOAT3(wiRenderer::GetDevice()->GetScreenWidth() * 0.5f, wiRenderer::GetDevice()->GetScreenHeight() * 0.75f, 0.f);
	sprite->effects.pivot = XMFLOAT2(0.5f, 0.5f);
	addSprite(sprite);
}
void DemoLoadingScreen::Update(float dt)
{
	Renderable2DComponent::Update(dt);
}
void DemoLoadingScreen::Compose()
{
	Renderable2DComponent::Compose();

	stringstream ss("");
	ss << "Loading: " << getPercentageComplete() << "%";
	wiFont(ss.str(), wiFontProps(wiRenderer::GetDevice()->GetScreenWidth() / 2, wiRenderer::GetDevice()->GetScreenHeight() / 2, 30, WIFALIGN_CENTER, WIFALIGN_CENTER)).Draw(GRAPHICSTHREAD_IMMEDIATE);
}


HelloWorldDemo::HelloWorldDemo(){
	wiSprite* image;
	image = new wiSprite("HelloWorldDemo/HelloWorld.png",&Content);
	image->effects.siz = XMFLOAT2(400, 200);
	image->effects.pos = XMFLOAT3((float)wiRenderer::GetDevice()->GetScreenWidth()/2, (float)wiRenderer::GetDevice()->GetScreenHeight()/2, 0);
	image->effects.pivot = XMFLOAT2(0.5f, 0.5f);
	image->anim.rot = 0.01f;

	addSprite(image);

	float screenW = (float)wiRenderer::GetDevice()->GetScreenWidth();
	float screenH = (float)wiRenderer::GetDevice()->GetScreenHeight();

	button.SetName("BUTTON1");
	button.OnClick([&](wiEventArgs args) {
		static int clicks = 0;
		clicks++;
		stringstream ss("");
		ss << "Button clicks: " << clicks;
		label.SetText(ss.str());
	});
	button.SetPos(XMFLOAT2(screenW *0.5f - 100, screenH*0.8f));
	button.SetSize(XMFLOAT2(200, 50));
	button.SetText("Click me!");
	GetGUI().AddWidget(&button);

	slider = wiSlider(12, 45, 0.5f, 23.0f, "SLIDER1");
	slider.SetText("Drag me: ");
	slider.SetPos(XMFLOAT2(screenW*0.5f - 200, screenH*0.55f));
	slider.SetSize(XMFLOAT2(400, 50));
	slider.OnSlide([&](wiEventArgs args) {
		stringstream ss("");
		ss << "Slider value: " << args.fValue << endl;
		label.SetText(ss.str());
	});
	GetGUI().AddWidget(&slider);

	label.SetName("LABEL1");
	label.SetPos(XMFLOAT2(screenW / 2 - 300, screenH*0.3f));
	label.SetSize(XMFLOAT2(600, 100));
	label.SetText("Label text");
	GetGUI().AddWidget(&label);

	wiSlider* slider1 = new wiSlider(0, 100, 20);
	slider1->SetSize(XMFLOAT2(100, 20));
	slider1->SetPos(XMFLOAT2(screenW *0.75f, screenH*0.12f));
	slider1->OnSlide([&](wiEventArgs args) {
		stringstream ss("");
		ss << "Slider1 value: " << args.fValue << endl;
		label.SetText(ss.str());
	});
	GetGUI().AddWidget(slider1);

	wiSlider* slider2 = new wiSlider(50, 230, 11, 230 - 50);
	slider2->SetSize(XMFLOAT2(100, 20));
	slider2->SetPos(XMFLOAT2(screenW *0.75f, screenH*0.18f));
	slider2->OnSlide([&](wiEventArgs args) {
		stringstream ss("");
		ss << "Slider2 value: " << args.fValue << endl;
		label.SetText(ss.str());
	});
	GetGUI().AddWidget(slider2);

	wiSlider* slider3 = new wiSlider(10, 4000, 300, 3);
	slider3->SetSize(XMFLOAT2(100, 20));
	slider3->SetPos(XMFLOAT2(screenW *0.75f, screenH*0.24f));
	slider3->OnSlide([&](wiEventArgs args) {
		stringstream ss("");
		ss << "Slider3 value: " << args.fValue << endl;
		label.SetText(ss.str());
	});
	GetGUI().AddWidget(slider3);
}
HelloWorldDemo::~HelloWorldDemo(){
	Renderable2DComponent::~Renderable2DComponent();
}
void HelloWorldDemo::Update(float dt){
	Renderable2DComponent::Update(dt);
}
void HelloWorldDemo::Compose(){
	Renderable2DComponent::Compose();
}

void BasicModelDemo::Initialize()
{
	setFXAAEnabled(false);
	setColorGradingEnabled(false);
	setBloomEnabled(false);
	setLightShaftsEnabled(false);
	setLensFlareEnabled(false);
	setVolumeLightsEnabled(false);
	setEmitterParticlesEnabled(false);
	setHairParticlesEnabled(false);

	__super::Initialize();
}
void BasicModelDemo::Load()
{
	__super::Load();

	model = wiRenderer::LoadModel("BasicModelDemo/barrel/", "barrel", XMMatrixTranslation(0,2.2f,0));
	wiRenderer::LoadDefaultLighting();
	wiRenderer::FinishLoading();
}
void BasicModelDemo::Start(){
	__super::Start();
	//wiRenderer::objects.front()->translation_rest.y = 2.2f;
}
void BasicModelDemo::Update(float dt){
	__super::Update(dt);

	model->RotateRollPitchYaw(XMFLOAT3(0, XM_2PI * dt * 0.1f, 0));
	//wiRenderer::objects.front()->transform(XMMatrixRotationRollPitchYaw(0, 0.001f*XM_2PI, 0));
	//wiRenderer::UpdateRenderInfo(wiRenderer::getImmediateContext());
}

void SkinnedModelDemo::Initialize()
{
	setFXAAEnabled(false);
	setColorGradingEnabled(false);
	setBloomEnabled(false);
	setLightShaftsEnabled(false);
	setLensFlareEnabled(false);
	setVolumeLightsEnabled(false);
	setEmitterParticlesEnabled(false);
	setHairParticlesEnabled(false);

	ForwardRenderableComponent::Initialize();
}
void SkinnedModelDemo::Load()
{
	ForwardRenderableComponent::Load();

	wiRenderer::LoadModel("SkinnedModelDemo/", "girl", XMMatrixTranslation(0,0,2));
	wiRenderer::LoadDefaultLighting();
	wiRenderer::FinishLoading();
}
void SkinnedModelDemo::Start(){
	ForwardRenderableComponent::Start();
	//wiRenderer::objects.front()->translation_rest.y = 2.2f;

	//wiRenderer::armatures.back()->ChangeAction("ArmatureIdle");

	wiRenderer::SetToDrawDebugBoneLines(true);
}
void SkinnedModelDemo::Stop()
{
	ForwardRenderableComponent::Stop();

	wiRenderer::SetToDrawDebugBoneLines(false);
}

void EmittedParticleDemo::Initialize()
{
	setFXAAEnabled(false);
	setColorGradingEnabled(false);
	setHairParticlesEnabled(false);
	setVolumeLightsEnabled(false);
	setLightShaftsEnabled(false);
	setLensFlareEnabled(false);
	setAlphaParticleDownSample(0.5f);
	setAdditiveParticleDownSample(0.5f);

	ForwardRenderableComponent::Initialize();
}
void EmittedParticleDemo::Load()
{
	ForwardRenderableComponent::Load();

	model = wiRenderer::LoadModel("EmitterParticleDemo/", "emitter", XMMatrixTranslation(0,2.2f,0));
	wiRenderer::LoadDefaultLighting();
	wiRenderer::FinishLoading();
}
void EmittedParticleDemo::Start(){
	ForwardRenderableComponent::Start();
	//for (Object* o : wiRenderer::objects)
	//	o->translation_rest.y = 2.2f;
}

void HairParticleDemo::Initialize()
{
	setFXAAEnabled(false);
	setColorGradingEnabled(false);
	setBloomEnabled(false);
	setEmitterParticlesEnabled(false);
	setVolumeLightsEnabled(false);
	setLightShaftsEnabled(false);
	setLensFlareEnabled(false);

	ForwardRenderableComponent::Initialize();
}
void HairParticleDemo::Load()
{
	ForwardRenderableComponent::Load();

	wiRenderer::LoadModel("HairParticleDemo/", "hair");
	wiRenderer::LoadDefaultLighting();
	wiRenderer::FinishLoading();
	wiHairParticle::Settings(8, 14, 28); //LOD levels (close, mid, distant)
}
void HairParticleDemo::Start(){
	ForwardRenderableComponent::Start();
}

void RigidBodyDemo::Initialize()
{
	setFXAAEnabled(false);
	setColorGradingEnabled(false);
	setBloomEnabled(false);
	setLightShaftsEnabled(false);
	setLensFlareEnabled(false);
	setVolumeLightsEnabled(false);
	setEmitterParticlesEnabled(false);
	setHairParticlesEnabled(false);

	ForwardRenderableComponent::Initialize();
}
void RigidBodyDemo::Load()
{
	ForwardRenderableComponent::Load();

	wiRenderer::LoadModel("RigidBodyDemo/", "rigidScene");
	wiRenderer::LoadDefaultLighting();
	wiRenderer::FinishLoading();
}
void RigidBodyDemo::Start(){
	ForwardRenderableComponent::Start();
}

void SoftBodyDemo::Initialize()
{
	setFXAAEnabled(false);
	setColorGradingEnabled(false);
	setBloomEnabled(false);
	setLightShaftsEnabled(false);
	setLensFlareEnabled(false);
	setVolumeLightsEnabled(false);
	setEmitterParticlesEnabled(false);
	setHairParticlesEnabled(false);

	ForwardRenderableComponent::Initialize();
}
void SoftBodyDemo::Load()
{
	ForwardRenderableComponent::Load();

	wiRenderer::LoadModel("SoftBodyDemo/", "flag");
	wiRenderer::LoadDefaultLighting();
	wiRenderer::FinishLoading();
}
void SoftBodyDemo::Start(){
	ForwardRenderableComponent::Start();
}

void DeferredLightDemo::Initialize()
{
	setSSAOEnabled(true);
	setSSREnabled(false);

	DeferredRenderableComponent::Initialize();
}
void DeferredLightDemo::Load()
{
	DeferredRenderableComponent::Load();

	wiRenderer::LoadModel("DeferredSceneDemo/lightBenchmark/", "lightBenchmark");
	wiRenderer::FinishLoading();
	wiRenderer::SetEnviromentMap((Texture2D*)Content.add("DeferredSceneDemo/lightBenchmark/env.dds"));
	wiRenderer::SetColorGrading((Texture2D*)Content.add("DeferredSceneDemo/lightBenchmark/colorGrading.dds"));
	wiHairParticle::Settings(20, 50, 200);
}
void DeferredLightDemo::Start(){
	DeferredRenderableComponent::Start();
}

void DeferredSceneDemo::Initialize()
{
#ifndef BUILD_PHONE
	setSSAOEnabled(true);
	setSSREnabled(false);
	setEyeAdaptionEnabled(true);
#else
	this->setSSAOEnabled(false);
	this->setSSREnabled(false);
	this->setEyeAdaptionEnabled(false);
	this->setFXAAEnabled(false);
	this->setLightShaftsEnabled(false);
	this->setReflectionQuality(0.2f);
	this->setTessellationEnabled(false);
	this->setMotionBlurEnabled(false);
	this->setSSSEnabled(false);
	//this->setHairParticlesEnabled(false);
#endif
	setReflectionsEnabled(true);

	DeferredRenderableComponent::Initialize();
}
void DeferredSceneDemo::Load()
{
	DeferredRenderableComponent::Load();

	wiRenderer::LoadModel("DeferredSceneDemo/instanceBenchmark2/", "instanceBenchmark2");
	wiRenderer::FinishLoading();
	wiRenderer::SetEnviromentMap((Texture2D*)Content.add("DeferredSceneDemo/instanceBenchmark2/env.dds"));
	wiRenderer::SetColorGrading((Texture2D*)Content.add("DeferredSceneDemo/instanceBenchmark2/colorGrading.dds"));
	wiHairParticle::Settings(20, 50, 120);
}
void DeferredSceneDemo::Start(){
	DeferredRenderableComponent::Start();
}

void ForwardSceneDemo::Initialize()
{
#ifdef BUILD_PHONE
	this->setSSAOEnabled(false);
	this->setSSREnabled(false);
	this->setEyeAdaptionEnabled(false);
	this->setFXAAEnabled(false);
	this->setLightShaftsEnabled(false);
	this->setReflectionQuality(0.2f);
	this->setTessellationEnabled(false);
	this->setMotionBlurEnabled(false);
	this->setSSSEnabled(false);
	this->setHairParticleAlphaCompositionEnabled(false);
#endif
	setReflectionsEnabled(true);

	__super::Initialize();
}
void ForwardSceneDemo::Load()
{
	__super::Load();

	wiRenderer::LoadModel("DeferredSceneDemo/instanceBenchmark2/", "instanceBenchmark2");
	wiRenderer::FinishLoading();
	wiRenderer::SetEnviromentMap((Texture2D*)Content.add("DeferredSceneDemo/instanceBenchmark2/env.dds"));
	wiRenderer::SetColorGrading((Texture2D*)Content.add("DeferredSceneDemo/instanceBenchmark2/colorGrading.dds"));
	wiHairParticle::Settings(20, 50, 120);
}
void ForwardSceneDemo::Start(){
	__super::Start();
}

void SSRTestDemo::Initialize()
{
	setSSAOEnabled(true);
	setSSREnabled(true);

	DeferredRenderableComponent::Initialize();
}
void SSRTestDemo::Load()
{
	DeferredRenderableComponent::Load();

	wiRenderer::LoadModel("DeferredSceneDemo/ssrtest/", "ssrtest");
	wiRenderer::FinishLoading();
	wiRenderer::SetEnviromentMap((Texture2D*)Content.add("DeferredSceneDemo/ssrtest/env.dds"));
	wiRenderer::SetColorGrading((Texture2D*)Content.add("DeferredSceneDemo/instanceBenchmark2/colorGrading.dds"));
	wiHairParticle::Settings(20, 50, 200);
}
void SSRTestDemo::Start(){
	DeferredRenderableComponent::Start();
}

void SkinTestDemo::Initialize()
{
	setSSAOEnabled(true);
	setSSREnabled(true);

	DeferredRenderableComponent::Initialize();
}
void SkinTestDemo::Load()
{
	DeferredRenderableComponent::Load();

	wiRenderer::LoadModel("SkinTestDemo/", "leeperrysmith");
	wiRenderer::FinishLoading();
}
void SkinTestDemo::Start(){
	DeferredRenderableComponent::Start();
}

